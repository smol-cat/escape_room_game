// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundBase.h"
#include "escape_room/Velocity.h"
#include "OnVelocity.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_ROOM_API UOnVelocity : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOnVelocity();

	UVelocity* velocityComponent = nullptr;
	UPROPERTY(EditAnywhere)
		double invokingMinimum = 100.0;
	UPROPERTY(EditAnywhere)
		USoundBase* sound = nullptr;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> spawnee;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
