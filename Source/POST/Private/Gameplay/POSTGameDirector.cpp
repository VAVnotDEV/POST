#include "Gameplay/POSTGameDirector.h"
#include "Gameplay/POSTRunSaveGame.h"
#include "Kismet/GameplayStatics.h"

APOSTGameDirector::APOSTGameDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APOSTGameDirector::BeginPlay()
{
    Super::BeginPlay();
    LoadProgress();
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
}
