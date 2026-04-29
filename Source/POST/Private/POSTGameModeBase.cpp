// Copyright Epic Games, Inc. All Rights Reserved.


#include "POSTGameModeBase.h"
#include "Player\POSTCharacter.h"
#include "Player\POSTPlayerController.h"

APOSTGameModeBase::APOSTGameModeBase()
{
	DefaultPawnClass = APOSTCharacter::StaticClass();
	PlayerControllerClass = APOSTPlayerController::StaticClass();
}