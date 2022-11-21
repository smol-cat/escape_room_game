// Fill out your copyright notice in the Description page of Project Settings.


#include "SmoothRotate.h"

// Sets default values for this component's properties
USmoothRotate::USmoothRotate()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USmoothRotate::BeginPlay()
{
	Super::BeginPlay();

	rotationSpeed = ClampVector(rotationSpeed, FVector(0,0,0), FVector(100,100,100));
	rotationSpeed = rotationSpeed * FVector(0.01, 0.01, 0.01);
	
}


// Called every frame
void USmoothRotate::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(isRotating)
	{
		auto currentRotation = GetOwner()->GetActorRotation();

		auto newRotation = GetOwner()->GetActorRotation();
		
		newRotation.Roll = FMath::Lerp<float>(newRotation.Roll, rotationTarget.Roll, rotationSpeed.X);
		newRotation.Pitch = FMath::Lerp<float>(newRotation.Pitch, rotationTarget.Pitch, rotationSpeed.Y);
		newRotation.Yaw = FMath::Lerp<float>(newRotation.Yaw, rotationTarget.Yaw, rotationSpeed.Z);

		double sum = 0.0;
		sum += currentRotation.Roll - newRotation.Roll;
		sum += currentRotation.Pitch - newRotation.Pitch;
		sum += currentRotation.Yaw - newRotation.Yaw;

		GetOwner()->SetActorRotation(newRotation);
		
		if(sum < 0.0001)
		{
			isRotating = false;
		}
	}
}

void USmoothRotate::SmoothRotate(FRotator r, FVector rSpeed)
{
	this->rotationTarget = r;
	this->rotationSpeed = rSpeed;
	isRotating = true;
}