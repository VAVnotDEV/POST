#include "Gameplay/POSTGameDirector.h"

#include "Components/POSTRadioComponent.h"
#include "Gameplay/POSTAnomaly.h"
#include "Gameplay/POSTCycleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/POSTCharacter.h"
#include "EngineUtils.h"
#include "POSTLog.h"

APOSTGameDirector::APOSTGameDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APOSTGameDirector::BeginPlay()
{
    Super::BeginPlay();
    CacheWorldReferences();
}

void APOSTGameDirector::CacheWorldReferences()
{
    Player = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    CycleManager = Cast<APOSTCycleManager>(UGameplayStatics::GetActorOfClass(this, APOSTCycleManager::StaticClass()));

    Anomalies.Reset();
    ActiveAnomalies.Reset();
    for (TActorIterator<APOSTAnomaly> It(GetWorld()); It; ++It)
    {
        Anomalies.Add(*It);
    }
}

int32 APOSTGameDirector::GetRebootCount() const
{
    return CycleManager ? CycleManager->GetTotalCycles() : 0;
}

bool APOSTGameDirector::IsRebootInProgress() const
{
    return CycleManager && CycleManager->IsRebootInProgress();
}

bool APOSTGameDirector::SetStoryStage(EPOSTStoryStage NewStage)
{
    if (StoryStage == NewStage)
    {
        return false;
    }

    const EPOSTStoryStage OldStage = StoryStage;
    StoryStage = NewStage;
    OnStoryStageChanged.Broadcast(OldStage, StoryStage);
    return true;
}

bool APOSTGameDirector::AdvanceStoryStage(EPOSTStoryStage ExpectedCurrentStage, EPOSTStoryStage NewStage)
{
    return StoryStage == ExpectedCurrentStage && SetStoryStage(NewStage);
}

void APOSTGameDirector::RegisterDeath(EPOSTDeathCause Cause)
{
    LastDeathCause = Cause;

    if (!CycleManager)
    {
        CycleManager = Cast<APOSTCycleManager>(UGameplayStatics::GetActorOfClass(this, APOSTCycleManager::StaticClass()));
    }

    if (!CycleManager)
    {
        UE_LOG(LogPOST, Error, TEXT("RegisterDeath: POSTCycleManager is missing from the level."));
        return;
    }

    OnRebooted.Broadcast(CycleManager->GetTotalCycles() + 1, Cause);
    CycleManager->EndCycle(Cause);
}

bool APOSTGameDirector::PlayRadioMessage(FName MessageId)
{
    if (!Player)
    {
        CacheWorldReferences();
    }
    if (!Player || !Player->GetRadioComponent())
    {
        return false;
    }

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
    if (!IsValid(Anomaly) || IsRebootInProgress())
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
        if (!IsValid(Active) || !Active->IsActive()) continue;
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
    if (!Player || IsRebootInProgress())
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

void APOSTGameDirector::StartPresenceEncounter()
{
    SetPresenceState(EPOSTPresenceState::Warning);
}

void APOSTGameDirector::StopPresenceEncounter()
{
    SetPresenceState(EPOSTPresenceState::Inactive);
}

void APOSTGameDirector::SetPresenceState(EPOSTPresenceState NewState)
{
    if (PresenceState == NewState)
    {
        return;
    }

    PresenceState = NewState;
    OnPresenceStateChanged.Broadcast(PresenceState);
}
