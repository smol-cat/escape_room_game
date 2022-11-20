// Fill out your copyright notice in the Description page of Project Settings.


#include "BrokenCup.h"

// Sets default values
ABrokenCup::ABrokenCup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABrokenCup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABrokenCup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

