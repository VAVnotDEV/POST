#include "Gameplay/POSTCycleManager.h"

#include "Components/POSTInteractionComponent.h"
#include "Gameplay/POSTMetaSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Player/POSTCharacter.h"
#include "TimerManager.h"

APOSTCycleManager::APOSTCycleManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APOSTCycleManager::BeginPlay()
{
    Super::BeginPlay();
    LoadMeta();
}

void APOSTCycleManager::EndCycle(EPOSTDeathCause Cause)
{
    if (bRebootInProgress)
    {
        return;
    }

    bRebootInProgress = true;
    LastDeathCause = Cause;
    ++TotalCycles;
    bFirstRebootCompleted = true;
    SaveMeta();

    FreezePlayer();
    OnCycleEnded.Broadcast(TotalCycles, Cause);
    OnCycleEndPresentation(Cause);

    GetWorldTimerManager().SetTimer(RebootTimer, this, &APOSTCycleManager::ReloadCurrentLevel, RebootDelay, false);
}

void APOSTCycleManager::FreezePlayer()
{
    if (APOSTCharacter* Player = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->DropCarriedActor();
        if (UPOSTInteractionComponent* Interaction = Player->FindComponentByClass<UPOSTInteractionComponent>())
        {
            Interaction->SetInteractionEnabled(false);
        }
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
    }
}

void APOSTCycleManager::ReloadCurrentLevel()
{
    const FName LevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
    if (!LevelName.IsNone())
    {
        UGameplayStatics::OpenLevel(this, LevelName);
        return;
    }

    bRebootInProgress = false;
}

void APOSTCycleManager::LoadMeta()
{
    if (!UGameplayStatics::DoesSaveGameExist(MetaSaveSlotName, SaveUserIndex))
    {
        return;
    }

    if (UPOSTMetaSaveGame* Save = Cast<UPOSTMetaSaveGame>(UGameplayStatics::LoadGameFromSlot(MetaSaveSlotName, SaveUserIndex)))
    {
        TotalCycles = FMath::Max(0, Save->TotalCycles);
        bFirstRebootCompleted = Save->bFirstRebootCompleted;
        LastDeathCause = Save->LastDeathCause;
    }
}

void APOSTCycleManager::SaveMeta()
{
    UPOSTMetaSaveGame* Save = Cast<UPOSTMetaSaveGame>(UGameplayStatics::CreateSaveGameObject(UPOSTMetaSaveGame::StaticClass()));
    if (!Save)
    {
        return;
    }

    Save->TotalCycles = TotalCycles;
    Save->bFirstRebootCompleted = bFirstRebootCompleted;
    Save->LastDeathCause = LastDeathCause;
    UGameplayStatics::SaveGameToSlot(Save, MetaSaveSlotName, SaveUserIndex);
}

void APOSTCycleManager::ResetMetaProgress()
{
    UGameplayStatics::DeleteGameInSlot(MetaSaveSlotName, SaveUserIndex);
    TotalCycles = 0;
    bFirstRebootCompleted = false;
    LastDeathCause = EPOSTDeathCause::Unknown;
}
