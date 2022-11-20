// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GrabbableMeshActor.h"
#include "Components/StaticMeshComponent.h"

AGrabbableMeshActor::AGrabbableMeshActor(const FObjectInitializer& ObjInitializer)
{
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
	GetStaticMeshComponent()->SetCollisionProfileName("PhysicsActor");
	bIsInteractable = true;
}

void AGrabbableMeshActor::Grab(USceneComponent* InComponent)
{
	if (bIsInteractable)
	{
		GetStaticMeshComponent()->SetSimulatePhysics(false);
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		AttachmentRules.LocationRule = EAttachmentRule::SnapToTarget;
		AttachmentRules.RotationRule = EAttachmentRule::KeepWorld;
		AttachmentRules.ScaleRule = EAttachmentRule::KeepWorld;
		AttachmentRules.bWeldSimulatedBodies = true;

		GetStaticMeshComponent()->AttachToComponent(InComponent, AttachmentRules, "hand_Socket");
	}
}

USceneComponent* AGrabbableMeshActor::GetGrabbingComponent()
{
	return RootComponent->GetAttachParent();
}

float AGrabbableMeshActor::GetRequiredForce()
{
	return 0.0f;
}

void AGrabbableMeshActor::Release()
{
	if (bIsInteractable)
	{
		GetStaticMeshComponent()->SetSimulatePhysics(true);
		FDetachmentTransformRules DetachmentRules(
			EDetachmentRule::KeepWorld,
			EDetachmentRule::KeepWorld,
			EDetachmentRule::KeepWorld,
			true
		);
		GetStaticMeshComponent()->DetachFromComponent(DetachmentRules);
	}
}
