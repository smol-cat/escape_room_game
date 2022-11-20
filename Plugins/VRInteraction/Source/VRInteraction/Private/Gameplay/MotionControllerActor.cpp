// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MotionControllerActor.h"
#include "Components/SceneComponent.h"
#include "MotionControllerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Gameplay/Grabbable.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Components/WidgetComponent.h"

// Sets default values
AMotionControllerActor::AMotionControllerActor(const FObjectInitializer& ObjInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>("MotionController");
	MotionController->SetupAttachment(Scene);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>("HandMesh");
	HandMesh->SetupAttachment(MotionController);

	ArcDirection = CreateDefaultSubobject<UArrowComponent>("ArcDirection");
	ArcDirection->SetupAttachment(HandMesh);
	ArcDirection->ArrowSize = 0.2F;

	ArcSpline = CreateDefaultSubobject<USplineComponent>("ArcSpline");
	ArcSpline->SetupAttachment(HandMesh);

	GrabSphere = CreateDefaultSubobject<USphereComponent>("GrabSphere");
	GrabSphere->SetupAttachment(HandMesh);
	GrabSphere->SetSphereRadius(10.0F);

	WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>("WidgetInteraction");
	WidgetInteraction->SetupAttachment(HandMesh);
	WidgetInteraction->InteractionDistance = 4.0F;
	WidgetInteraction->SetVisibility(false);

	ArcEndPoint = CreateDefaultSubobject<UStaticMeshComponent>("ArcEndPoint");
	ArcEndPoint->SetupAttachment(Scene);
	ArcEndPoint->SetGenerateOverlapEvents(false);
	ArcEndPoint->SetCollisionProfileName("NoCollision");

	TeleportCylinder = CreateDefaultSubobject<UStaticMeshComponent>("TeleportCylinder");
	TeleportCylinder->SetupAttachment(Scene);
	TeleportCylinder->SetCollisionProfileName("NoCollision");

	Ring = CreateDefaultSubobject<UStaticMeshComponent>("Ring");
	Ring->SetupAttachment(TeleportCylinder);

	Arrow = CreateDefaultSubobject<UStaticMeshComponent>("Arrow");
	Arrow->SetupAttachment(TeleportCylinder);
	Arrow->SetCollisionProfileName("NoCollision");

	TeleportLaunchVelocity = 900.0F;

	Hand = EControllerHand::Right;
	ForceFeedback = nullptr;

	AttachedActor = nullptr;
	bIsTeleporterActive = false;
	SplineMeshes = TArray<USplineMeshComponent*>();
	bLastFrameWasValidDestination = false;
	TeleportDestination = FVector();
	bIsValidTeleportDestination = false;
	GripState = EGripState::GS_Open;
	bWantsToGrip = false;
	TeleportRotation = FRotator();
	InitialControllerRotation = FRotator();
	PendingActor = nullptr;
}

// Called when the game starts or when spawned
void AMotionControllerActor::BeginPlay()
{
	Super::BeginPlay();

	TeleportCylinder->SetVisibility(false, true);

	SetHandiness(Hand);
}

// Called every frame
void AMotionControllerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update hand grip state
	if (AttachedActor != nullptr || bWantsToGrip)
	{
		GripState = EGripState::GS_Grab;
	}
	else
	{
		AActor* ActorNearHand = GetActorNearHand();
		if (ActorNearHand != nullptr)
		{
			GripState = EGripState::GS_CanGrab;
		}
		else if (bWantsToGrip)
		{
			GripState = EGripState::GS_Grab;
		}
		else
		{
			GripState = EGripState::GS_Open;
		}
	}
	// Allow colliding with objects only when gripping
	HandMesh->SetCollisionEnabled((GripState == EGripState::GS_Grab) ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);

	// Teleportation Arc
	ClearArc();
	if (bIsTeleporterActive)
	{
		TArray<FVector> TracePoints;
		FVector NavMeshLocation;
		FVector TraceLocation;
		bIsValidTeleportDestination = TraceTeleportDestination(TracePoints, NavMeshLocation, TraceLocation);
		TeleportCylinder->SetVisibility(bIsValidTeleportDestination, true);
		UWorld* World = GetWorld();
		if (World != nullptr)
		{
			FHitResult Hit;
			FCollisionObjectQueryParams ObjectParams;
			ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
			FCollisionQueryParams QueryParams;
			QueryParams.bTraceComplex = false;
			QueryParams.AddIgnoredActor(this);
			if (AttachedActor != nullptr)
			{
				QueryParams.AddIgnoredActor(AttachedActor);
			}
			World->LineTraceSingleByObjectType(Hit, NavMeshLocation, NavMeshLocation - FVector(0.0F, 0.0F, -200.0F), ObjectParams, QueryParams);

			TeleportCylinder->SetWorldLocation(
				(Hit.bBlockingHit) ? Hit.ImpactPoint : NavMeshLocation, false, nullptr, ETeleportType::TeleportPhysics);
		}

		// Rumble Controller if found valid teleport destination
		if ((bIsValidTeleportDestination && !bLastFrameWasValidDestination) || (bLastFrameWasValidDestination && !bIsValidTeleportDestination))
		{
			RumbleController(0.3F);
		}

		bLastFrameWasValidDestination = bIsValidTeleportDestination;

		UpdateArcSpline(bIsValidTeleportDestination, TracePoints);
		UpdateArcEndpoint(TraceLocation, bIsValidTeleportDestination);
	}

	FHitResult Hit = WidgetInteraction->GetLastHitResult();
	if (Hit.bBlockingHit)
	{
		if (Hit.Distance <= WidgetInteraction->InteractionDistance * 0.5F && !bIsPressing)
		{
			bIsPressing = true;
			WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
		}
		else if (bIsPressing && Hit.Distance > WidgetInteraction->InteractionDistance * 0.5F)
		{
			bIsPressing = false;
			WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
		}
	}
	else
	{
		bIsPressing = false;
	}
}

AActor* AMotionControllerActor::GetActorNearHand()
{
	TArray<AActor*> OverlapActors;
	GrabSphere->GetOverlappingActors(OverlapActors, AActor::StaticClass());

	float NearestOverlap = 10000.0F;
	AActor* NearestOverlappingActor = nullptr;

	for (int i = 0; i < OverlapActors.Num(); i++)
	{
		if (Cast<IGrabbable>(OverlapActors[i]) != nullptr)
		{
			float Distance = (OverlapActors[i]->GetActorLocation() - GrabSphere->GetComponentLocation()).Size();
			if (Distance < NearestOverlap)
			{
				NearestOverlappingActor = OverlapActors[i];
				NearestOverlap = Distance;
			}
		}
	}

	return NearestOverlappingActor;
}

AActor* AMotionControllerActor::ReleaseActor()
{
	bWantsToGrip = false;
	AActor* ReleasedActor = AttachedActor;
	if (AttachedActor != nullptr)
	{
		IGrabbable* Grabbable = Cast<IGrabbable>(AttachedActor);
		if (Grabbable->GetGrabbingComponent() == HandMesh)
		{
			Grabbable->Release();
			RumbleController(0.05F);
		}
		else
		{
			// The hand wasn't holding anything
			// so the released actor is set to null
			ReleasedActor = nullptr;
		}
		AttachedActor = nullptr;
		PendingActor = nullptr;
	}
	OnReleased.Broadcast(this, ReleasedActor);
	return nullptr;
}

void AMotionControllerActor::GrabActor()
{
	bWantsToGrip = true;
	PendingActor = GetActorNearHand();
	PerformGrab();
}

AActor* AMotionControllerActor::PerformGrab()
{
	if (PendingActor != nullptr && AttachedActor == nullptr)
	{
		IGrabbable* Grabbable = Cast<IGrabbable>(PendingActor);
		AttachedActor = PendingActor;
		// Doesn't take mass into account.
		Grabbable->Grab(HandMesh);
	}
	OnGrabbed.Broadcast(this, AttachedActor);
	return AttachedActor;
}

void AMotionControllerActor::ActivateTeleporter()
{
	bIsTeleporterActive = true;
	TeleportCylinder->SetVisibility(true, true);
	InitialControllerRotation = MotionController->GetComponentRotation();
}

void AMotionControllerActor::DisableTeleporter()
{
	if (bIsTeleporterActive)
	{
		bIsTeleporterActive = false;
		TeleportCylinder->SetVisibility(false, true);
		ArcEndPoint->SetVisibility(false, false);
	}
}

bool AMotionControllerActor::TraceTeleportDestination(TArray<FVector>& OutTracePoints, FVector& OutNavMeshLocation, FVector& OutTraceLocation)
{
	FPredictProjectilePathParams PredictParams;
	FPredictProjectilePathResult PredictResult;

	PredictParams.StartLocation = ArcDirection->GetComponentLocation();
	PredictParams.LaunchVelocity = ArcDirection->GetForwardVector() * TeleportLaunchVelocity;
	PredictParams.bTraceComplex = false;
	PredictParams.bTraceWithCollision = true;
	PredictParams.ProjectileRadius = 0.0F;
	PredictParams.ObjectTypes.Add(TEnumAsByte<EObjectTypeQuery>(EObjectTypeQuery::ObjectTypeQuery1));
	PredictParams.MaxSimTime = 2.0F;
	PredictParams.SimFrequency = 30.0F;
	PredictParams.OverrideGravityZ = 0.0F;
	if (AttachedActor != nullptr)
	{
		PredictParams.ActorsToIgnore.Add(AttachedActor);
	}

	bool bTraceSuccess = UGameplayStatics::PredictProjectilePath(this, PredictParams, PredictResult);
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);
	FNavLocation NavMeshLocation;
	bool bNavProjectSuccess = NavSystem->ProjectPointToNavigation(PredictResult.HitResult.Location, NavMeshLocation, FVector::OneVector * 500.0F);

	OutNavMeshLocation = NavMeshLocation.Location;
	OutTraceLocation = PredictResult.HitResult.Location;

	OutTracePoints.Empty(PredictResult.PathData.Num());

	for (auto PathData : PredictResult.PathData)
	{
		OutTracePoints.Add(PathData.Location);
	}

	return bTraceSuccess && bNavProjectSuccess;
}

void AMotionControllerActor::ClearArc()
{
	for (auto SplineMesh : SplineMeshes)
	{
		SplineMesh->DestroyComponent();
	}
	SplineMeshes.Empty();
	ArcSpline->ClearSplinePoints();
}

void AMotionControllerActor::UpdateArcSpline(bool bInFoundValidLocation, TArray<FVector>& InSplinePoints)
{
	if (!bInFoundValidLocation)
	{
		InSplinePoints.Empty();
		FVector Start = ArcDirection->GetComponentLocation();
		InSplinePoints.Add(Start);
		InSplinePoints.Add(Start + ArcDirection->GetForwardVector() * 20.0F);
	}

	for (FVector Point : InSplinePoints)
	{
		ArcSpline->AddSplinePoint(Point, ESplineCoordinateSpace::Local);
	}
	ArcSpline->SetSplinePointType(InSplinePoints.Num() - 1, ESplinePointType::CurveClamped);

	for (int i = 0; i < ArcSpline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* SplineMesh = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), true, FTransform::Identity, false));
		SplineMeshes.Add(SplineMesh);
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetStaticMesh(ArcSegmentMesh);
		SplineMesh->SetStartAndEnd(
			InSplinePoints[i],
			ArcSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local),
			InSplinePoints[i + 1],
			ArcSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local)
		);
	}
}

void AMotionControllerActor::UpdateArcEndpoint(const FVector& InNewLocation, bool bInValidLocationFound)
{
	ArcEndPoint->SetVisibility(bIsTeleporterActive && bInValidLocationFound);
	ArcEndPoint->SetWorldLocation(InNewLocation, false, nullptr, ETeleportType::TeleportPhysics);

	FRotator DeviceOrientation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceOrientation, DevicePosition);

	Arrow->SetWorldRotation(UKismetMathLibrary::ComposeRotators(TeleportRotation, FRotator(0.0F, DeviceOrientation.Yaw, 0.0F)));
}

void AMotionControllerActor::GetTeleportDestination(FVector& OutLocation, FRotator& OutRotation)
{
	FRotator DeviceOrientation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceOrientation, DevicePosition);

	DevicePosition.Z = 0.0F;

	DevicePosition = TeleportRotation.Quaternion() * DevicePosition;
	OutLocation = TeleportCylinder->GetComponentLocation() - DevicePosition;
	OutRotation = TeleportRotation;
}

void AMotionControllerActor::RumbleController(float InIntensity)
{
	if (ForceFeedback != nullptr)
	{
		APawn* Pawn = Cast<APawn>(GetOwner());
		if (Pawn != nullptr)
		{
			APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
			if (PC != nullptr)
			{
				PC->PlayHapticEffect(ForceFeedback, Hand, InIntensity);
			}
		}
	}
}

void AMotionControllerActor::GrabSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IGrabbable>(OtherActor) != nullptr)
	{
		RumbleController(0.05F);
	}
}

void AMotionControllerActor::SetHandiness(EControllerHand InHand)
{
	Hand = InHand;
	FVector HandScale = HandMesh->GetComponentScale();
	if (Hand == EControllerHand::Left)
	{
		HandScale.Z = FMath::Abs(HandScale.Z) * -1.0F;
		HandMesh->SetWorldScale3D(HandScale);
		WidgetInteraction->PointerIndex = 1;
		MotionController->SetTrackingMotionSource("Left");
	}
	else if (Hand == EControllerHand::Right)
	{
		HandScale.Z = FMath::Abs(HandScale.Z);
		HandMesh->SetWorldScale3D(HandScale);
		WidgetInteraction->PointerIndex = 0;
		MotionController->SetTrackingMotionSource("Right");
	}
}

void AMotionControllerActor::SetTeleportRotation(const FRotator& InRotator)
{
	TeleportRotation = InRotator;
}