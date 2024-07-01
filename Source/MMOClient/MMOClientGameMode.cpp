// Copyright Epic Games, Inc. All Rights Reserved.

#include "MMOClientGameMode.h"
#include "GameObject/MMOClientCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/DefaultPawn.h"
AMMOClientGameMode::AMMOClientGameMode()
{
	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
	//DefaultPawnClass = ADefaultPawn::StaticClass();
}
