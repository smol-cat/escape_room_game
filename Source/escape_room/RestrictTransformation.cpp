// Fill out your copyright notice in the Description page of Project Settings.


#include "RestrictTransformation.h"

// Sets default values for this component's properties
URestrictTransformation::URestrictTransformation()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void URestrictTransformation::BeginPlay()
{
	Super::BeginPlay();
	const auto root = this->GetOwner()->GetRootComponent();
	const auto location = root->GetComponentLocation();
	rotation = root->GetComponentRotation();
	scale = root->GetComponentScale();

	locationMin += location;
	locationMax += location;

	rotationMin += rotation;
	rotationMax += rotation;
}


// Called every frame
void URestrictTransformation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto currentRotation = GetOwner()->GetActorRotation();
	if(currentRotation.Pitch > rotationMax.Pitch) currentRotation.Pitch = rotationMax.Pitch;
	else if(currentRotation.Pitch < rotationMin.Pitch) currentRotation.Pitch = rotationMin.Pitch;
	if(currentRotation.Yaw > rotationMax.Yaw) currentRotation.Yaw = rotationMax.Yaw;
	else if(currentRotation.Yaw < rotationMin.Yaw) currentRotation.Yaw = rotationMin.Yaw;
	if(currentRotation.Roll > rotationMax.Roll) currentRotation.Roll = rotationMax.Roll;
	else if(currentRotation.Roll < rotationMin.Roll) currentRotation.Roll = rotationMin.Roll;
	
	GetOwner()->SetActorLocation(ClampVector(GetOwner()->GetActorLocation(), locationMin, locationMax));
	GetOwner()->SetActorRotation(currentRotation);
	
	// this->GetOwner()->SetActorRotation(rotation);
	// this->GetOwner()->SetActorScale3D(scale);
}

