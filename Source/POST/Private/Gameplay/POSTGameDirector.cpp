#include "Gameplay/POSTGameDirector.h"

#include "Components/POSTRadioComponent.h"
#include "Gameplay/POSTAnomaly.h"
#include "Gameplay/POSTRunSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Player/POSTCharacter.h"
#include "EngineUtils.h"
#include "TimerManager.h"

APOSTGameDirector::APOSTGameDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APOSTGameDirector::BeginPlay()
{
    Super::BeginPlay();
    LoadProgress();
    CacheWorldReferences();
    GetWorldTimerManager().SetTimer(DirectorTimer, this, &APOSTGameDirector::UpdateDirector, DirectorUpdateInterval, true);
}

void APOSTGameDirector::CacheWorldReferences()
{
    Player = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    Anomalies.Reset();
    for (TActorIterator<APOSTAnomaly> It(GetWorld()); It; ++It)
    {
        Anomalies.Add(*It);
    }
}

void APOSTGameDirector::UpdateDirector()
{
    if (!Player)
    {
        Player = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    }
    if (!Player) return;

    const float NewThreat = 1.0f - FMath::Clamp(FVector::Distance(Player->GetActorLocation(), EntityLocation) / FMath::Max(ThreatRadius, 1.0f), 0.0f, 1.0f);
    if (!FMath::IsNearlyEqual(NewThreat, ThreatLevel, 0.01f))
    {
        ThreatLevel = NewThreat;
        OnThreatChanged.Broadcast(ThreatLevel);
    }

    if (UPOSTRadioComponent* Radio = Player->GetRadioComponent())
    {
        Radio->SetInterference(ThreatLevel);
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
    ++RebootCount;
    LastDeathCause = Cause;
    WorldResourceMultiplier = FMath::Max(MinimumResourceMultiplier, WorldResourceMultiplier - ResourceLossPerReboot);
    GeneratorReliabilityMultiplier = FMath::Max(MinimumReliabilityMultiplier, GeneratorReliabilityMultiplier - ReliabilityLossPerReboot);
    SaveProgress();
    OnRebooted.Broadcast(RebootCount, Cause);
    if (Cause == EPOSTDeathCause::Cold) OnColdAftereffectRequested();
    OnWorldRebootRequested(Cause);
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

void APOSTGameDirector::SetEntityLocation(FVector NewLocation)
{
    EntityLocation = NewLocation;
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

bool APOSTGameDirector::TryActivateNearbyAnomaly()
{
    if (!Player || ThreatLevel < MinimumThreatForAnomaly) return false;

    TArray<APOSTAnomaly*> Candidates;
    for (APOSTAnomaly* Anomaly : Anomalies)
    {
        if (IsValid(Anomaly) && !Anomaly->IsActive()) Candidates.Add(Anomaly);
    }
    if (Candidates.Num() == 0) return false;

    Candidates.Sort([this](const APOSTAnomaly& A, const APOSTAnomaly& B)
    {
        return FVector::DistSquared(A.GetActorLocation(), Player->GetActorLocation()) < FVector::DistSquared(B.GetActorLocation(), Player->GetActorLocation());
    });

    const int32 PoolSize = FMath::Min(3, Candidates.Num());
    return Candidates[FMath::RandRange(0, PoolSize - 1)]->ActivateAnomaly();
}

bool APOSTGameDirector::SaveProgress()
{
    UPOSTRunSaveGame* Save = Cast<UPOSTRunSaveGame>(UGameplayStatics::CreateSaveGameObject(UPOSTRunSaveGame::StaticClass()));
    if (!Save) return false;
    Save->StoryStage = StoryStage;
    Save->RebootCount = RebootCount;
    Save->WorldResourceMultiplier = WorldResourceMultiplier;
    Save->GeneratorReliabilityMultiplier = GeneratorReliabilityMultiplier;
    Save->LastDeathCause = LastDeathCause;
    return UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, SaveUserIndex);
}

bool APOSTGameDirector::LoadProgress()
{
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex)) return false;
    UPOSTRunSaveGame* Save = Cast<UPOSTRunSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
    if (!Save) return false;
    StoryStage = Save->StoryStage;
    RebootCount = Save->RebootCount;
    WorldResourceMultiplier = Save->WorldResourceMultiplier;
    GeneratorReliabilityMultiplier = Save->GeneratorReliabilityMultiplier;
    LastDeathCause = Save->LastDeathCause;
    return true;
}

void APOSTGameDirector::ResetProgress()
{
    UGameplayStatics::DeleteGameInSlot(SaveSlotName, SaveUserIndex);
    StoryStage = EPOSTStoryStage::Arrival;
    RebootCount = 0;
    WorldResourceMultiplier = 1.0f;
    GeneratorReliabilityMultiplier = 1.0f;
    LastDeathCause = EPOSTDeathCause::Unknown;
    PlayedRadioMessages.Reset();
}
