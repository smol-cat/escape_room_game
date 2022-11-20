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

	if (velocityComponent != nullptr && sound != nullptr && velocityComponent->speed >= invokingMinimum) {
		//UGameplayStatics::PlaySound2D(this, sound);
		
		auto* s = GetWorld()->SpawnActor<AActor>(spawnee, this->GetOwner()->GetActorLocation(), this->GetOwner()->GetActorRotation());
		for (auto child : this->GetOwner()->Children)
		{
			GetWorld()->DestroyActor(child);
		}
		GetWorld()->DestroyActor(this->GetOwner());
		
		FString msg = FString::Printf(TEXT("OnVelocity::SpeedInvoke: %s"), *FString::SanitizeFloat(velocityComponent->speed));
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Red, msg);
	}
}

