#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTCycleManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOSTCycleEnded, int32, TotalCycles, EPOSTDeathCause, Cause);

UCLASS(Blueprintable)
class POST_API APOSTCycleManager : public AActor
{
    GENERATED_BODY()

public:
    APOSTCycleManager();
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintAssignable, Category="POST|Cycle")
    FPOSTCycleEnded OnCycleEnded;

    UFUNCTION(BlueprintCallable, Category="POST|Cycle")
    void EndCycle(EPOSTDeathCause Cause);

    UFUNCTION(BlueprintPure, Category="POST|Cycle")
    int32 GetTotalCycles() const { return TotalCycles; }

    UFUNCTION(BlueprintPure, Category="POST|Cycle")
    bool HasCompletedFirstReboot() const { return bFirstRebootCompleted; }

    UFUNCTION(BlueprintPure, Category="POST|Cycle")
    bool IsRebootInProgress() const { return bRebootInProgress; }

    UFUNCTION(BlueprintCallable, Category="POST|Cycle")
    void ResetMetaProgress();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Cycle")
    FString MetaSaveSlotName = TEXT("POST_Meta");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Cycle")
    int32 SaveUserIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Cycle", meta=(ClampMin="0.0"))
    float RebootDelay = 1.5f;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Cycle")
    void OnCycleEndPresentation(EPOSTDeathCause Cause);

private:
    void LoadMeta();
    void SaveMeta();
    void ReloadCurrentLevel();
    void FreezePlayer();

    FTimerHandle RebootTimer;
    bool bRebootInProgress = false;
    int32 TotalCycles = 0;
    bool bFirstRebootCompleted = false;
    EPOSTDeathCause LastDeathCause = EPOSTDeathCause::Unknown;
};
