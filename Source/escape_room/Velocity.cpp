// Fill out your copyright notice in the Description page of Project Settings.


#include "Velocity.h"

// Sets default values for this component's properties
UVelocity::UVelocity()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UVelocity::BeginPlay()
{
	Super::BeginPlay();
	auto location = this->GetOwner()->GetActorLocation();
	timeVectorArray.Init({FDateTime::Now(), location}, queueCapacity);
	index = 0;
}


// Called every frame
void UVelocity::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (initFull == queueCapacity)
	{
		double totalDistance = 0.0;

		auto previous = timeVectorArray[index];
		auto startTime = previous.Key;

		for (int i = (index + 1) % queueCapacity; i != index; i = (i + 1) % queueCapacity)
		{
			auto current = timeVectorArray[i];
			auto distance = FVector::Dist(current.Value, previous.Value);
			totalDistance += distance;
			previous = current;
		}

		auto endTime = previous.Key;
		auto timeSpan = endTime - startTime;

		double duration = timeSpan.GetTotalSeconds();

		speed = totalDistance / duration;
	}
	else
	{
		initFull++;
	}

	auto location = this->GetOwner()->GetActorLocation();
	timeVectorArray[index] = {FDateTime::Now(), location};
	index = (index + 1) % queueCapacity;
}
