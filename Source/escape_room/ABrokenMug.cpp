// Fill out your copyright notice in the Description page of Project Settings.


#include "ABrokenMug.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AABrokenMug::AABrokenMug()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
		
	for(int i = 0; i <= 26; i++)
	{
		FString s = FString::Printf(TEXT("/Game/Blender/Cups/Cups_Cup_cell_%03d"), i);
		ConstructorHelpers::FObjectFinder<UStaticMesh> cell((TEXT("%s"), *s));
		if (cell.Succeeded())
		{
			auto visualMesh = CreateDefaultSubobject<UStaticMeshComponent>((TEXT("%s"), *s));
			visualMesh->SetupAttachment(RootComponent);
			visualMesh->SetStaticMesh(cell.Object);
			visualMesh->SetWorldLocation(FVector(0.0f, 0.0f, 0.0f));
			meshes.Emplace(visualMesh);
		}
	}
}

// Called when the game starts or when spawned
void AABrokenMug::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AABrokenMug::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

