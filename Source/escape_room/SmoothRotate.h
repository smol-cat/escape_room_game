// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SmoothRotate.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_ROOM_API USmoothRotate : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USmoothRotate();

	UPROPERTY(VisibleAnywhere)
	FRotator rotationTarget;
	UPROPERTY(VisibleAnywhere)
	FVector rotationSpeed;

	bool isRotating = false;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SmoothRotate(FRotator r, FVector rSpeed);
		
};
