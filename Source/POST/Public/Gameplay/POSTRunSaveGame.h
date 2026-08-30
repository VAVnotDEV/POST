#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTRunSaveGame.generated.h"

UCLASS()
class POST_API UPOSTRunSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, SaveGame) EPOSTStoryStage StoryStage = EPOSTStoryStage::Arrival;
    UPROPERTY(BlueprintReadWrite, SaveGame) int32 RebootCount = 0;
    UPROPERTY(BlueprintReadWrite, SaveGame) EPOSTDeathCause LastDeathCause = EPOSTDeathCause::Unknown;
    UPROPERTY(BlueprintReadWrite, SaveGame) bool bHasSavedWorldTime = false;
    UPROPERTY(BlueprintReadWrite, SaveGame) int32 SavedDay = 1;
    UPROPERTY(BlueprintReadWrite, SaveGame) int32 SavedHours = 21;
    UPROPERTY(BlueprintReadWrite, SaveGame) int32 SavedMinutes = 0;
    UPROPERTY(BlueprintReadWrite, SaveGame) int32 SavedSeconds = 0;
    UPROPERTY(BlueprintReadWrite, SaveGame) TArray<FName> PlayedRadioMessageIds;
};
