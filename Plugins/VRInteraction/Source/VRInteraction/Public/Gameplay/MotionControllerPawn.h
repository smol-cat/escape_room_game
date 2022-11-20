// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MotionControllerPawn.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FBeginTeleport, class AMotionControllerActor*)
DECLARE_MULTICAST_DELEGATE_TwoParams(FFinishTeleportDelegate, const FTransform&, const FTransform&)

UCLASS()
class VRINTERACTION_API AMotionControllerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMotionControllerPawn(const FObjectInitializer& ObjInitializer);

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller Pawn")
		class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller Pawn")
		USceneComponent* VROrigin;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller Pawn")
		class UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller Pawn")
		class USphereComponent* HeadSphere;

	UPROPERTY(EditAnywhere, Category = "Motion Controller Pawn")
		TSubclassOf<AMotionControllerActor> LeftHandClass;

	UPROPERTY(EditAnywhere, Category = "Motion Controller Pawn")
		TSubclassOf<AMotionControllerActor> RightHandClass;

	UPROPERTY(EditAnywhere, Category = "Motion Controller Pawn")
		TSubclassOf<class UFadeUserWidget> FadeWidgetClass;

	UPROPERTY()
		UFadeUserWidget* FadeWidget;
	
private:
	AMotionControllerActor* LeftController;
	AMotionControllerActor* RightController;

	float FadeOutDuration;
	float FadeInDuration;

	bool bIsTeleporting;
	FLinearColor TeleportFadeColor;

	float ThumbDeadZone;

	bool bIsRightStickDown;
	bool bIsLeftStickDown;

	float DefaultPlayerHeight;

	float bUseControllerRollToRotate;

	FTimerHandle TimerHandle_TeleportingTimer;

public:
	FBeginTeleport OnBeginTeleport;
	FFinishTeleportDelegate OnFinishTeleport;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ExecuteTeleportation(AMotionControllerActor* InMotionController);

	void GrabLeft();
	void ReleaseLeft();

	void GrabRight();
	void ReleaseRight();

	void TeleportLeft();
	void ReleaseTeleportLeft();

	void TeleportRight();
	void ReleaseTeleportRight();

	FRotator GetRotationFromInput(float InUpAxis, float InRightAxis, AMotionControllerActor* InMotionController);

	UFUNCTION()
		void FinishTeleport(AMotionControllerActor* InMotionController);

	UFUNCTION(BlueprintPure, Category = "Motion Controller Pawn")
		FORCEINLINE UCameraComponent* GetCamera() { return PlayerCamera; }
	UFUNCTION(BlueprintPure, Category = "Motion Controller Pawn")
		FORCEINLINE AMotionControllerActor* GetLeftController() { return LeftController; }
	UFUNCTION(BlueprintPure, Category = "Motion Controller Pawn")
		FORCEINLINE AMotionControllerActor* GetRightController() { return RightController; }

	FORCEINLINE UFadeUserWidget* GetFade() { return FadeWidget; }

};
