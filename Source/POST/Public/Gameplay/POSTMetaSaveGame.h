#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTMetaSaveGame.generated.h"

UCLASS()
class POST_API UPOSTMetaSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, SaveGame, Category="POST|Meta")
    int32 TotalCycles = 0;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="POST|Meta")
    bool bFirstRebootCompleted = false;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="POST|Meta")
    EPOSTDeathCause LastDeathCause = EPOSTDeathCause::Unknown;
};
