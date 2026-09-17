#include "Gameplay/POSTGameDirector.h"

#include "Components/POSTRadioComponent.h"
#include "Gameplay/POSTAnomaly.h"
#include "Gameplay/POSTCycleManager.h"
#include "Gameplay/POSTEntity.h"
#include "Kismet/GameplayStatics.h"
#include "Player/POSTCharacter.h"
#include "EngineUtils.h"
#include "POSTLog.h"

APOSTGameDirector::APOSTGameDirector()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APOSTGameDirector::BeginPlay()
{
    Super::BeginPlay();
    CacheWorldReferences();
    ScheduleNextManifestation();
}

void APOSTGameDirector::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateEntityPresence(DeltaTime);
}

void APOSTGameDirector::CacheWorldReferences()
{
    Player = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    CycleManager = Cast<APOSTCycleManager>(UGameplayStatics::GetActorOfClass(this, APOSTCycleManager::StaticClass()));
    Entity = Cast<APOSTEntity>(UGameplayStatics::GetActorOfClass(this, APOSTEntity::StaticClass()));

    Anomalies.Reset();
    ActiveAnomalies.Reset();
    for (TActorIterator<APOSTAnomaly> It(GetWorld()); It; ++It)
    {
        Anomalies.Add(*It);
    }
}

void APOSTGameDirector::UpdateEntityPresence(float DeltaTime)
{
    if (!IsValid(Entity))
    {
        Entity = Cast<APOSTEntity>(UGameplayStatics::GetActorOfClass(this, APOSTEntity::StaticClass()));
    }

    if (!IsValid(Entity) || !IsValid(Player) || ManifestationRadius <= FullManifestationDistance)
    {
        EntityPresence = 0.0f;
        return;
    }

    const float DistanceToPlayer = FVector::Dist(Entity->GetActorLocation(), Player->GetActorLocation());
    EntityPresence = 1.0f - FMath::GetRangePct(FullManifestationDistance, ManifestationRadius, DistanceToPlayer);
    EntityPresence = FMath::Clamp(EntityPresence, 0.0f, 1.0f);

    if (EntityPresence <= KINDA_SMALL_NUMBER || IsRebootInProgress())
    {
        // Presence has left the outer radius. A future return gets a fresh random delay.
        ManifestationCountdown = -1.0f;
        return;
    }

    if (ManifestationCountdown < 0.0f)
    {
        ScheduleNextManifestation();
    }

    ManifestationCountdown -= DeltaTime;
    if (ManifestationCountdown > 0.0f)
    {
        return;
    }

    // Failure is intentional: silence is part of the system. We simply schedule another opportunity.
    TryActivateNearbyAnomaly();
    ScheduleNextManifestation();
}

void APOSTGameDirector::ScheduleNextManifestation()
{
    const float Presence = FMath::Clamp(EntityPresence, 0.0f, 1.0f);
    const float MinInterval = FMath::Lerp(FarManifestationMinInterval, NearManifestationMinInterval, Presence);
    const float MaxInterval = FMath::Lerp(FarManifestationMaxInterval, NearManifestationMaxInterval, Presence);
    ManifestationCountdown = FMath::FRandRange(FMath::Min(MinInterval, MaxInterval), FMath::Max(MinInterval, MaxInterval));
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
    if (!IsValid(Entity) || IsRebootInProgress())
    {
        return false;
    }

    ActiveAnomalies.RemoveAll([](const APOSTAnomaly* Anomaly)
    {
        return !IsValid(Anomaly) || !Anomaly->IsActive();
    });

    const float RadiusSq = FMath::Square(ManifestationRadius);
    TArray<APOSTAnomaly*> Candidates;

    for (APOSTAnomaly* Anomaly : Anomalies)
    {
        if (!IsValid(Anomaly) || !Anomaly->CanActivate())
        {
            continue;
        }

        // The anomaly belongs to the Entity's presence, not to a player trigger.
        // Only authored anomaly points physically reached by the outer presence are eligible.
        if (FVector::DistSquared(Anomaly->GetActorLocation(), Entity->GetActorLocation()) <= RadiusSq)
        {
            Candidates.Add(Anomaly);
        }
    }

    if (Candidates.Num() == 0)
    {
        return false;
    }

    // No "nearest anomaly" rule: that would create learnable scripts.
    // Every currently valid point inside the Entity's presence can be selected.
    while (Candidates.Num() > 0)
    {
        const int32 Index = FMath::RandRange(0, Candidates.Num() - 1);
        APOSTAnomaly* Candidate = Candidates[Index];
        Candidates.RemoveAtSwap(Index);

        if (Candidate->ActivateAnomaly())
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
