#include "Gameplay/POSTGameDirector.h"

#include "Components/POSTRadioComponent.h"
#include "Components/POSTInteractionComponent.h"
#include "Gameplay/POSTAnomaly.h"
#include "Gameplay/POSTRunSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Player/POSTCharacter.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "POSTGameState.h"

APOSTGameDirector::APOSTGameDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APOSTGameDirector::BeginPlay()
{
    Super::BeginPlay();
    LoadProgress();
    CacheWorldReferences();
    ApplySavedWorldState();
}

void APOSTGameDirector::CacheWorldReferences()
{
    Player = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    Anomalies.Reset();
    ActiveAnomalies.Reset();
    for (TActorIterator<APOSTAnomaly> It(GetWorld()); It; ++It)
    {
        Anomalies.Add(*It);
    }
}


void APOSTGameDirector::ApplySavedWorldState()
{
    if (!bHasSavedWorldTime)
    {
        return;
    }

    if (APOSTGameState* GameState = GetWorld() ? GetWorld()->GetGameState<APOSTGameState>() : nullptr)
    {
        GameState->SetGameTime(SavedDay, SavedHours, SavedMinutes, SavedSeconds);
    }
}

bool APOSTGameDirector::SetStoryStage(EPOSTStoryStage NewStage)
{
    if (StoryStage == NewStage) return false;
    const EPOSTStoryStage OldStage = StoryStage;
    StoryStage = NewStage;
    OnStoryStageChanged.Broadcast(OldStage, StoryStage);
    SaveProgress();
    return true;
}

bool APOSTGameDirector::AdvanceStoryStage(EPOSTStoryStage ExpectedCurrentStage, EPOSTStoryStage NewStage)
{
    return StoryStage == ExpectedCurrentStage && SetStoryStage(NewStage);
}

void APOSTGameDirector::RegisterDeath(EPOSTDeathCause Cause)
{
    if (bRebootInProgress)
    {
        return;
    }

    bRebootInProgress = true;
    ++RebootCount;
    LastDeathCause = Cause;
    SaveProgress();

    if (Player)
    {
        Player->DropCarriedActor();
        if (UPOSTInteractionComponent* Interaction = Player->FindComponentByClass<UPOSTInteractionComponent>())
        {
            Interaction->SetInteractionEnabled(false);
        }
    }

    if (bReloadCurrentLevelOnDeath)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            PC->SetIgnoreMoveInput(true);
            PC->SetIgnoreLookInput(true);
        }
    }

    OnRebooted.Broadcast(RebootCount, Cause);
    if (Cause == EPOSTDeathCause::Cold)
    {
        OnColdAftereffectRequested();
    }
    OnWorldRebootRequested(Cause);

    if (bReloadCurrentLevelOnDeath)
    {
        GetWorldTimerManager().SetTimer(RebootTimer, this, &APOSTGameDirector::ExecuteWorldReboot, RebootDelay, false);
    }
}

void APOSTGameDirector::ExecuteWorldReboot()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        bRebootInProgress = false;
        return;
    }

    const FName CurrentLevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
    if (CurrentLevelName.IsNone())
    {
        bRebootInProgress = false;
        return;
    }

    UGameplayStatics::OpenLevel(this, CurrentLevelName);
}

bool APOSTGameDirector::PlayRadioMessage(FName MessageId)
{
    if (!Player) CacheWorldReferences();
    if (!Player || !Player->GetRadioComponent()) return false;

    for (const FPOSTRadioMessage& Message : RadioMessages)
    {
        if (Message.MessageId != MessageId || !Message.Sound) continue;
        if (static_cast<uint8>(StoryStage) < static_cast<uint8>(Message.MinimumStage)) return false;
        if (Message.bPlayOnce && PlayedRadioMessages.Contains(MessageId)) return false;
        if (!Player->GetRadioComponent()->PlayMessage(MessageId, Message.Sound)) return false;
        if (Message.bPlayOnce) PlayedRadioMessages.Add(MessageId);
        return true;
    }
    return false;
}


bool APOSTGameDirector::ActivateAnomalyByName(FName ActorName)
{
    for (APOSTAnomaly* Anomaly : Anomalies)
    {
        if (IsValid(Anomaly) && Anomaly->GetFName() == ActorName)
        {
            return Anomaly->ActivateAnomaly();
        }
    }
    return false;
}

bool APOSTGameDirector::CanStartAnomaly(const APOSTAnomaly* Anomaly) const
{
    if (!IsValid(Anomaly) || bRebootInProgress)
    {
        return false;
    }

    if (GlobalAnomalyCooldown > 0.0f && LastAnomalyFinishedWorldTime >= 0.0f && GetWorld())
    {
        if (GetWorld()->GetTimeSeconds() - LastAnomalyFinishedWorldTime < GlobalAnomalyCooldown)
        {
            return false;
        }
    }

    int32 ValidActiveCount = 0;
    bool bBlockingAnomalyActive = false;
    for (APOSTAnomaly* Active : ActiveAnomalies)
    {
        if (!IsValid(Active) || !Active->IsActive())
        {
            continue;
        }

        ++ValidActiveCount;
        bBlockingAnomalyActive |= Active->BlocksOtherAnomalies();
    }

    if (ValidActiveCount >= FMath::Max(1, MaxConcurrentAnomalies))
    {
        return false;
    }

    return !bBlockingAnomalyActive && !(Anomaly->BlocksOtherAnomalies() && ValidActiveCount > 0);
}

void APOSTGameDirector::NotifyAnomalyStarted(APOSTAnomaly* Anomaly)
{
    if (IsValid(Anomaly))
    {
        ActiveAnomalies.AddUnique(Anomaly);
    }
}

void APOSTGameDirector::NotifyAnomalyStopped(APOSTAnomaly* Anomaly)
{
    ActiveAnomalies.Remove(Anomaly);
    LastAnomalyFinishedWorldTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
}

bool APOSTGameDirector::TryActivateNearbyAnomaly()
{
    if (!Player || bRebootInProgress)
    {
        return false;
    }

    ActiveAnomalies.RemoveAll([](const APOSTAnomaly* Anomaly)
    {
        return !IsValid(Anomaly) || !Anomaly->IsActive();
    });

    TArray<APOSTAnomaly*> Candidates;
    for (APOSTAnomaly* Anomaly : Anomalies)
    {
        if (IsValid(Anomaly) && Anomaly->CanActivate())
        {
            Candidates.Add(Anomaly);
        }
    }

    if (Candidates.Num() == 0)
    {
        return false;
    }

    Candidates.Sort([this](const APOSTAnomaly& A, const APOSTAnomaly& B)
    {
        return FVector::DistSquared(A.GetActorLocation(), Player->GetActorLocation()) <
               FVector::DistSquared(B.GetActorLocation(), Player->GetActorLocation());
    });

    const int32 PoolSize = FMath::Min(3, Candidates.Num());
    const int32 StartIndex = FMath::RandRange(0, PoolSize - 1);

    for (int32 Offset = 0; Offset < PoolSize; ++Offset)
    {
        const int32 CandidateIndex = (StartIndex + Offset) % PoolSize;
        if (Candidates[CandidateIndex]->ActivateAnomaly())
        {
            return true;
        }
    }

    return false;
}

bool APOSTGameDirector::SaveProgress()
{
    UPOSTRunSaveGame* Save = Cast<UPOSTRunSaveGame>(UGameplayStatics::CreateSaveGameObject(UPOSTRunSaveGame::StaticClass()));
    if (!Save) return false;
    Save->StoryStage = StoryStage;
    Save->RebootCount = RebootCount;

    Save->LastDeathCause = LastDeathCause;
    Save->PlayedRadioMessageIds = PlayedRadioMessages.Array();

    if (const APOSTGameState* GameState = GetWorld() ? GetWorld()->GetGameState<APOSTGameState>() : nullptr)
    {
        Save->bHasSavedWorldTime = true;
        Save->SavedDay = GameState->GetDay();
        Save->SavedHours = GameState->GetHours();
        Save->SavedMinutes = GameState->GetMinutes();
        Save->SavedSeconds = GameState->GetSeconds();
    }

    return UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, SaveUserIndex);
}

bool APOSTGameDirector::LoadProgress()
{
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex)) return false;
    UPOSTRunSaveGame* Save = Cast<UPOSTRunSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
    if (!Save) return false;
    StoryStage = Save->StoryStage;
    RebootCount = Save->RebootCount;
    LastDeathCause = Save->LastDeathCause;
    bHasSavedWorldTime = Save->bHasSavedWorldTime;
    SavedDay = FMath::Max(1, Save->SavedDay);
    SavedHours = FMath::Clamp(Save->SavedHours, 0, 23);
    SavedMinutes = FMath::Clamp(Save->SavedMinutes, 0, 59);
    SavedSeconds = FMath::Clamp(Save->SavedSeconds, 0, 59);

    PlayedRadioMessages.Reset();
    for (const FName MessageId : Save->PlayedRadioMessageIds)
    {
        if (!MessageId.IsNone())
        {
            PlayedRadioMessages.Add(MessageId);
        }
    }
    return true;
}

void APOSTGameDirector::ResetProgress()
{
    UGameplayStatics::DeleteGameInSlot(SaveSlotName, SaveUserIndex);
    StoryStage = EPOSTStoryStage::Arrival;
    RebootCount = 0;
    LastDeathCause = EPOSTDeathCause::Unknown;
    PlayedRadioMessages.Reset();
    bHasSavedWorldTime = false;
    SavedDay = 1;
    SavedHours = 21;
    SavedMinutes = 0;
    SavedSeconds = 0;
}
