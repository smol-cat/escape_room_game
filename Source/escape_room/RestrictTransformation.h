// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Math/Range.h"
#include "Math/Vector.h"
#include "RestrictTransformation.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_ROOM_API URestrictTransformation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URestrictTransformation();
	
	UPROPERTY(EditAnywhere)
	FVector locationMin;
	UPROPERTY(EditAnywhere)
	FVector locationMax;

	UPROPERTY(EditAnywhere)
	FRotator rotationMin;
	UPROPERTY(EditAnywhere)
	FRotator rotationMax;
	
	FRotator rotation;
	FVector scale;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
