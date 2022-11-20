// Fill out your copyright notice in the Description page of Project Settings.


#include "OnVelocity.h"

#include "ABrokenMug.h"
#include "BrokenCup.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UOnVelocity::UOnVelocity()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOnVelocity::BeginPlay()
{
	Super::BeginPlay();
	velocityComponent = GetOwner()->FindComponentByClass<UVelocity>();

}


// Called every frame
void UOnVelocity::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<AActor*> overlappingActors;
	this->GetOwner()->GetOverlappingActors(overlappingActors);
	int i = 0;
	for (auto OverlappingActor : overlappingActors)
	{
		i++;
		FString msg(((TEXT("Overlapping object %s"), OverlappingActor->GetFName().ToString())));
		GEngine->AddOnScreenDebugMessage(i, 1.0f, FColor::Red, msg);
	}

	if (velocityComponent != nullptr && sound != nullptr && velocityComponent->speed >= invokingMinimum) {
		UGameplayStatics::PlaySound2D(this, sound);

		auto* s = GetWorld()->SpawnActor<AActor>(spawnee, this->GetOwner()->GetActorLocation(), this->GetOwner()->GetActorRotation());
		TArray<AActor*> children;
		this->GetOwner()->GetWorld()->DestroyActor(keyActor);
		GetWorld()->DestroyActor(this->GetOwner());

		FString msg = FString::Printf(TEXT("OnVelocity::SpeedInvoke: %s"), *FString::SanitizeFloat(velocityComponent->speed));
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Red, msg);
	}
}

