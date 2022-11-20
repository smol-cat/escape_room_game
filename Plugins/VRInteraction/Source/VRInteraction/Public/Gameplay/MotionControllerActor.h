// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotionControllerActor.generated.h"

UENUM(Blueprintable)
enum class EGripState : uint8
{
	GS_Open,
	GS_CanGrab,
	GS_Grab
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FGrabDelegate, class AMotionControllerActor*, AActor*)
DECLARE_MULTICAST_DELEGATE_TwoParams(FReleaseDelegate, AMotionControllerActor*, AActor*)

UCLASS()
class VRINTERACTION_API AMotionControllerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMotionControllerActor(const FObjectInitializer& ObjInitializer);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		class UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion Controller")
		class USkeletalMeshComponent* HandMesh;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		class UArrowComponent* ArcDirection;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		class USplineComponent* ArcSpline;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		class USphereComponent* GrabSphere;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		class UWidgetInteractionComponent* WidgetInteraction;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		class UStaticMeshComponent* ArcEndPoint;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		UStaticMeshComponent* TeleportCylinder;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		UStaticMeshComponent* Ring;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		UStaticMeshComponent* Arrow;

	UPROPERTY(EditAnywhere, Category = "Motion Controller")
		EControllerHand Hand;

	UPROPERTY(EditAnywhere, Category = "Motion Controller")
		class UHapticFeedbackEffect_Base* ForceFeedback;

	UPROPERTY(EditAnywhere, Category = "Motion Controller")
		float TeleportLaunchVelocity;

	UPROPERTY(EditAnywhere, Category = "MotionController")
		UStaticMesh* ArcSegmentMesh;

private:

	AActor* AttachedActor;

	bool bIsTeleporterActive;

	TArray<class USplineMeshComponent*> SplineMeshes;

	bool bLastFrameWasValidDestination;

	bool bIsRoomScale;

	FVector TeleportDestination;

	bool bIsValidTeleportDestination;

	UPROPERTY(BlueprintReadOnly, Category = "Motion Controller", meta = (AllowPrivateAccess))
		EGripState GripState;

	bool bWantsToGrip;

	FRotator TeleportRotation;

	FRotator InitialControllerRotation;

	AActor* PendingActor;

	bool bIsPressing;

	FTimerHandle TimerHandle_WidgetPressTimeout;
	bool bCanBePressed;

public:

	FGrabDelegate OnGrabbed;
	FReleaseDelegate OnReleased;

private:

	void SetupRoomScaleOutline();
	void UpdateRoomScaleOutline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AActor* GetActorNearHand();

	// Returns released actor.
	AActor* ReleaseActor();

	void GrabActor();

	// Returns attached actor.
	AActor* PerformGrab();

	void ActivateTeleporter();

	void DisableTeleporter();

	bool TraceTeleportDestination(TArray<FVector>& OutTracePoints, FVector& OutNavMeshLocation, FVector& OutTraceLocation);

	void ClearArc();

	void UpdateArcSpline(bool bInFoundValidLocation, TArray<FVector>& InSplinePoints);

	void UpdateArcEndpoint(const FVector& InNewLocation, bool bInValidLocationFound);

	void GetTeleportDestination(FVector& OutLocation, FRotator& OutRotation);

	void RumbleController(float InIntensity);

	void GrabSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Motion Controller")
		USplineMeshComponent* AddSplineMeshComponentToArc();

	void SetHandiness(EControllerHand InHand);
	FORCEINLINE EControllerHand GetHandiness() { return Hand; }

	void SetTeleportRotation(const FRotator& InRotator);

	FORCEINLINE UMotionControllerComponent* GetMotionController() { return MotionController; }

	FORCEINLINE bool IsValidTeleportDestination() { return bIsValidTeleportDestination; }
	FORCEINLINE bool IsTeleporterActive() { return bIsTeleporterActive; }
	FORCEINLINE FRotator GetInitialControllerRotation() { return InitialControllerRotation; }
};
