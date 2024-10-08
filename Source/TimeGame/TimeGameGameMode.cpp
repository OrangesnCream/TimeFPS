// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeGameGameMode.h"
#include "TimeGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATimeGameGameMode::ATimeGameGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
