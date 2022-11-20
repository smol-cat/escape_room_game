// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Grabbable.h"
#include "GrabbableMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTION_API AGrabbableMeshActor : public AStaticMeshActor, public IGrabbable
{
	GENERATED_BODY()
public:
	AGrabbableMeshActor(const FObjectInitializer& ObjInitializer);

protected:

	UPROPERTY(EditAnywhere, Category = "VR")
		bool bIsInteractable;

private:
	bool bIsGrabbed;

public:
	virtual void Grab(class USceneComponent* InComponent) override;
	virtual USceneComponent* GetGrabbingComponent() override;
	virtual float GetRequiredForce() override;
	virtual void Release() override;
};
