// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Queue.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Velocity.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_ROOM_API UVelocity: public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVelocity();


		TArray<TPair<FDateTime, FVector>> timeVectorArray;
		uint32 index;
	UPROPERTY(EditAnywhere)
		uint32 queueCapacity = 1;
	UPROPERTY(VisibleAnywhere)
		double speed = 0.0;

	uint32 initFull = 0;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "ZZZ")
	float GetSpeed();
};
