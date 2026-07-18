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
    UPROPERTY(BlueprintReadWrite, SaveGame) float WorldResourceMultiplier = 1.0f;
    UPROPERTY(BlueprintReadWrite, SaveGame) float GeneratorReliabilityMultiplier = 1.0f;
    UPROPERTY(BlueprintReadWrite, SaveGame) EPOSTDeathCause LastDeathCause = EPOSTDeathCause::Unknown;
};
