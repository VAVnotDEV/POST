// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#include "POSTGameModeBase.h"

#include "Gameplay/POSTCycleManager.h"
#include "Kismet/GameplayStatics.h"
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

void APOSTGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (!UGameplayStatics::GetActorOfClass(this, APOSTCycleManager::StaticClass()))
    {
        GetWorld()->SpawnActor<APOSTCycleManager>(APOSTCycleManager::StaticClass());
    }
}
