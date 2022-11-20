// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MotionControllerPawn.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/MotionControllerActor.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Gameplay/FadeUserWidget.h"
#include "TimerManager.h"

// Sets default values
AMotionControllerPawn::AMotionControllerPawn(const FObjectInitializer& ObjInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	VROrigin = CreateDefaultSubobject<USceneComponent>("VROrigin");
	VROrigin->SetupAttachment(RootComponent);

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	PlayerCamera->SetupAttachment(VROrigin);

	HeadSphere = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	HeadSphere->SetupAttachment(PlayerCamera);
	HeadSphere->SetSphereRadius(25.0F);
	HeadSphere->SetGenerateOverlapEvents(true);
	HeadSphere->SetCollisionProfileName("Pawn");
	HeadSphere->SetCanEverAffectNavigation(false);

	LeftHandClass = AMotionControllerActor::StaticClass();
	RightHandClass = AMotionControllerActor::StaticClass();

	FadeOutDuration = 0.1F;
	FadeInDuration = 0.2F;
	bIsTeleporting = false;
	TeleportFadeColor = FLinearColor(0.0F, 0.0F, 0.0F, 1.0F);
	ThumbDeadZone = 0.7F;
	bIsRightStickDown = false;
	bIsLeftStickDown = false;
	DefaultPlayerHeight = 180.0F;
	bUseControllerRollToRotate = false;
}

// Called when the game starts or when spawned
void AMotionControllerPawn::BeginPlay()
{
	Super::BeginPlay();
	UHeadMountedDisplayFunctionLibrary::EnableHMD(true);
	FName DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		AttachmentRules.bWeldSimulatedBodies = false;
		AttachmentRules.ScaleRule = EAttachmentRule::KeepWorld;

		LeftController = World->SpawnActor<AMotionControllerActor>(LeftHandClass, SpawnParams);
		LeftController->SetHandiness(EControllerHand::Left);
		LeftController->AttachToComponent(VROrigin, AttachmentRules);

		RightController = World->SpawnActor<AMotionControllerActor>(RightHandClass, SpawnParams);
		RightController->SetHandiness(EControllerHand::Right);
		RightController->AttachToComponent(VROrigin, AttachmentRules);
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FadeWidget = CreateWidget<UFadeUserWidget, APlayerController>(PC, FadeWidgetClass);
		FadeWidget->AddToViewport();
		FadeWidget->FadeIn(1.0F);
	}
}

// Called every frame
void AMotionControllerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (LeftController->IsTeleporterActive())
	{
		float UpAxis = InputComponent->GetAxisValue("MotionControllerThumbLeft_Y");
		float RightAxis = InputComponent->GetAxisValue("MotionControllerThumbLeft_X");
		LeftController->SetTeleportRotation(
			GetRotationFromInput(
				UpAxis,
				RightAxis,
				LeftController
			)
		);
	}

	if (RightController->IsTeleporterActive())
	{
		float UpAxis = InputComponent->GetAxisValue("MotionControllerThumbRight_Y");
		float RightAxis = InputComponent->GetAxisValue("MotionControllerThumbRight_X");
		RightController->SetTeleportRotation(
			GetRotationFromInput(
				UpAxis,
				RightAxis,
				RightController
			)
		);
	}
}

// Called to bind functionality to input
void AMotionControllerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("GrabLeft", EInputEvent::IE_Pressed, this, &AMotionControllerPawn::GrabLeft);
	PlayerInputComponent->BindAction("GrabLeft", EInputEvent::IE_Released, this, &AMotionControllerPawn::ReleaseLeft);

	PlayerInputComponent->BindAction("GrabRight", EInputEvent::IE_Pressed, this, &AMotionControllerPawn::GrabRight);
	PlayerInputComponent->BindAction("GrabRight", EInputEvent::IE_Released, this, &AMotionControllerPawn::ReleaseRight);

	PlayerInputComponent->BindAction("TeleportLeft", EInputEvent::IE_Pressed, this, &AMotionControllerPawn::TeleportLeft);
	PlayerInputComponent->BindAction("TeleportLeft", EInputEvent::IE_Released, this, &AMotionControllerPawn::ReleaseTeleportLeft);

	PlayerInputComponent->BindAction("TeleportRight", EInputEvent::IE_Pressed, this, &AMotionControllerPawn::TeleportRight);
	PlayerInputComponent->BindAction("TeleportRight", EInputEvent::IE_Released, this, &AMotionControllerPawn::ReleaseTeleportRight);

	PlayerInputComponent->BindAxis("MotionControllerThumbLeft_X");
	PlayerInputComponent->BindAxis("MotionControllerThumbLeft_Y");

	PlayerInputComponent->BindAxis("MotionControllerThumbRight_X");
	PlayerInputComponent->BindAxis("MotionControllerThumbRight_Y");
}

void AMotionControllerPawn::ExecuteTeleportation(AMotionControllerActor* InMotionController)
{
	if (!bIsTeleporting)
	{
		if (InMotionController->IsValidTeleportDestination())
		{
			bIsTeleporting = true;
			/*APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC != nullptr)
			{
				PC->PlayerCameraManager->StartCameraFade(0.0F, 1.0F, FadeOutDuration, TeleportFadeColor, false, true);
				InMotionController->DisableTeleporter();

				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, FName("FinishTeleport"), InMotionController);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_TeleportingTimer, TimerDelegate, FadeOutDuration, false);
			}*/
			if (FadeWidget)
			{
				FadeWidget->FadeOut(FadeOutDuration);
				InMotionController->DisableTeleporter();

				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, FName("FinishTeleport"), InMotionController);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_TeleportingTimer, TimerDelegate, FadeOutDuration, false);
			}
		}
		else
		{
			InMotionController->DisableTeleporter();
		}
	}
}

void AMotionControllerPawn::GrabLeft()
{
	LeftController->GrabActor();
}

void AMotionControllerPawn::ReleaseLeft()
{
	LeftController->ReleaseActor();
}

void AMotionControllerPawn::GrabRight()
{
	RightController->GrabActor();
}

void AMotionControllerPawn::ReleaseRight()
{
	RightController->ReleaseActor();
}

void AMotionControllerPawn::TeleportLeft()
{
	LeftController->ActivateTeleporter();
	RightController->DisableTeleporter();
}

void AMotionControllerPawn::ReleaseTeleportLeft()
{
	if (LeftController->IsTeleporterActive())
	{
		ExecuteTeleportation(LeftController);
	}
}

void AMotionControllerPawn::TeleportRight()
{
	RightController->ActivateTeleporter();
	LeftController->DisableTeleporter();
}

void AMotionControllerPawn::ReleaseTeleportRight()
{
	if (RightController->IsTeleporterActive())
	{
		ExecuteTeleportation(RightController);
	}
}

FRotator AMotionControllerPawn::GetRotationFromInput(float InUpAxis, float InRightAxis, AMotionControllerActor* InMotionController)
{
	FRotator ActorRot = GetActorRotation();

	if (bUseControllerRollToRotate)
	{
		FTransform T1;
		T1.SetRotation(InMotionController->GetInitialControllerRotation().Quaternion());
		FTransform T2 = InMotionController->GetMotionController()->GetComponentTransform();

		FTransform T3 = UKismetMathLibrary::MakeRelativeTransform(T2, T1);

		return FRotator(0.0F,
			T3.GetRotation().Rotator().Yaw * 3.0F +
			ActorRot.Yaw,
			0.0F);
	}
	if (FMath::Abs(InUpAxis) + FMath::Abs(InRightAxis) >= ThumbDeadZone)
	{
		FVector ThumbstickVector(InUpAxis, InRightAxis, 0.0F);
		ThumbstickVector.Normalize();

		ThumbstickVector = ActorRot.Quaternion().RotateVector(ThumbstickVector);

		return ThumbstickVector.ToOrientationRotator();
	}
	return GetActorRotation();
}

void AMotionControllerPawn::FinishTeleport(AMotionControllerActor* InMotionController)
{
	if (bIsTeleporting)
	{
		FVector TeleportLocation;
		FRotator TeleportRotation;
		InMotionController->GetTeleportDestination(TeleportLocation, TeleportRotation);

		OnFinishTeleport.Broadcast(
			GetActorTransform(),
			FTransform(TeleportRotation, TeleportLocation, FVector::OneVector)
		);

		TeleportTo(TeleportLocation, TeleportRotation);

		/*APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC != nullptr)
		{
			PC->PlayerCameraManager->StartCameraFade(1.0F, 0.0F, FadeInDuration, TeleportFadeColor, false, false);
		}*/
		if (FadeWidget)
		{
			FadeWidget->FadeIn(FadeInDuration);
		}

		bIsTeleporting = false;
	}
}
