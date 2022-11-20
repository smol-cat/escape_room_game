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
}


// Called every frame
void URestrictTransformation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// auto currentLocation = GetOwner()->GetActorLocation();
	// if(currentLocation.X > locationMax.X) currentLocation.X = locationMax.X;
	// else if(currentLocation.X < locationMin.X) currentLocation.X = locationMin.X;
	// if(currentLocation.Y > locationMax.Y) currentLocation.Y = locationMax.Y;
	// else if(currentLocation.Y < locationMin.Y) currentLocation.Y = locationMin.Y;
	// if(currentLocation.Z > locationMax.Z) currentLocation.Z = locationMax.Z;
	// else if(currentLocation.Z < locationMin.Z) currentLocation.Z = locationMin.Z;

	
	GetOwner()->SetActorLocation(ClampVector(GetOwner()->GetActorLocation(), locationMin, locationMax));
	this->GetOwner()->SetActorRotation(rotation);
	// this->GetOwner()->SetActorScale3D(scale);
}

