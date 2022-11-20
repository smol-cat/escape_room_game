// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeUserWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FFadedInDelegate)
DECLARE_MULTICAST_DELEGATE(FFadedOutDelegate)

/**
 * User widget for fading during teleports.
 * Should be used for mobile platforms only
 * when Mobile HDR setting is off.
 */
UCLASS()
class VRINTERACTION_API UFadeUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFadeUserWidget(const FObjectInitializer& ObjInitializer);

protected:
	// Widget animation for fading. Duration has to be 1 s;
	// Has to fade out (turn to black) when playing forward.
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		class UWidgetAnimation* FadeAnimation;

private:
	bool bIsFadingOut;

	float FadePauseDuration;
	float FadeInDuration;

	bool bFadeInAfterFadingOut;

protected:

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

public:
	void FadeIn(float InDuration);
	void FadeOut(float InDuration);

	void FadeOutAndIn(float InFadeOutDuration, float InFadeInDuration, float InPauseDuration = 0.0F);

	FFadedInDelegate OnFadedIn;
	FFadedOutDelegate OnFadedOut;
};
