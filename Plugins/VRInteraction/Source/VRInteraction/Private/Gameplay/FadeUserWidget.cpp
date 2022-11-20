// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FadeUserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "TimerManager.h"

UFadeUserWidget::UFadeUserWidget(const FObjectInitializer& ObjInitializer) : Super(ObjInitializer)
{
	FadeAnimation = nullptr;
	bIsFadingOut = false;

	FadePauseDuration = 0.0F;
	FadeInDuration = 0.0F;

	bFadeInAfterFadingOut = false;
}

void UFadeUserWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (bIsFadingOut)
	{
		OnFadedOut.Broadcast();
		FTimerHandle FadeInTimer;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UFadeUserWidget::FadeIn, FadeInDuration);
		GetWorld()->GetTimerManager().SetTimer(FadeInTimer, TimerDelegate, FadePauseDuration, false);
		bFadeInAfterFadingOut = false;
	}
	else
	{
		OnFadedIn.Broadcast();
	}
}

void UFadeUserWidget::FadeIn(float InDuration)
{
	if (InDuration > 0.0F)
	{
		bIsFadingOut = false;
		PlayAnimationReverse(FadeAnimation, 1.0F / InDuration);
	}
}

void UFadeUserWidget::FadeOut(float InDuration)
{
	if (InDuration > 0.0F)
	{
		bIsFadingOut = true;
		PlayAnimationForward(FadeAnimation, 1.0F / InDuration);
	}
}

void UFadeUserWidget::FadeOutAndIn(float InFadeOutDuration, float InFadeInDuration, float InPause)
{
	FadeOut(InFadeOutDuration);
	FadePauseDuration = InPause;
	FadeInDuration = InFadeInDuration;
	bFadeInAfterFadingOut = true;
}
