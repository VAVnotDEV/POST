// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "POSTGameModeBase.h"
#include "Player/POSTCharacter.h"
#include "Player/POSTPlayerController.h"
#include "POSTGameState.h"
#include "UI/POSTHUD.h"

APOSTGameModeBase::APOSTGameModeBase()
{
	DefaultPawnClass = APOSTCharacter::StaticClass();
	PlayerControllerClass = APOSTPlayerController::StaticClass();
	GameStateClass = APOSTGameState::StaticClass();
	HUDClass = APOSTHUD::StaticClass();
}