// Copyright Epic Games, Inc. All Rights Reserved.

#include "escape_roomGameMode.h"
#include "escape_roomCharacter.h"
#include "UObject/ConstructorHelpers.h"

Aescape_roomGameMode::Aescape_roomGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
