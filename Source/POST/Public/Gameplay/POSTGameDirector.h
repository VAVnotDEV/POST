#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTGameDirector.generated.h"

class APOSTAnomaly;
class APOSTCharacter;
class APOSTCycleManager;
class USoundBase;

USTRUCT(BlueprintType)
struct FPOSTRadioMessage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName MessageId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) USoundBase* Sound = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EPOSTStoryStage MinimumStage = EPOSTStoryStage::Arrival;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bPlayOnce = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOSTStoryStageChanged, EPOSTStoryStage, OldStage, EPOSTStoryStage, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOSTRebooted, int32, RebootCount, EPOSTDeathCause, Cause);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTPresenceStateChanged, EPOSTPresenceState, NewState);

UCLASS(Blueprintable)
class POST_API APOSTGameDirector : public AActor
{
    GENERATED_BODY()

public:
    APOSTGameDirector();
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintAssignable, Category="POST|Director") FPOSTStoryStageChanged OnStoryStageChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Director") FPOSTRebooted OnRebooted;

    UFUNCTION(BlueprintPure, Category="POST|Director") EPOSTStoryStage GetStoryStage() const { return StoryStage; }
    UFUNCTION(BlueprintPure, Category="POST|Director") int32 GetRebootCount() const;
    UFUNCTION(BlueprintPure, Category="POST|Director") EPOSTDeathCause GetLastDeathCause() const { return LastDeathCause; }
    UFUNCTION(BlueprintPure, Category="POST|Director") bool IsRebootInProgress() const;

    UFUNCTION(BlueprintCallable, Category="POST|Director") bool SetStoryStage(EPOSTStoryStage NewStage);
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool AdvanceStoryStage(EPOSTStoryStage ExpectedCurrentStage, EPOSTStoryStage NewStage);

    // Compatibility entry point for existing systems (cold, accidents). Actual reboot belongs to CycleManager.
    UFUNCTION(BlueprintCallable, Category="POST|Director") void RegisterDeath(EPOSTDeathCause Cause);

    UFUNCTION(BlueprintCallable, Category="POST|Director|Radio") bool PlayRadioMessage(FName MessageId);
    UFUNCTION(BlueprintCallable, Category="POST|Director|Anomaly") bool ActivateAnomalyByName(FName ActorName);
    UFUNCTION(BlueprintCallable, Category="POST|Director|Anomaly") bool TryActivateNearbyAnomaly();
    UFUNCTION(BlueprintPure, Category="POST|Director|Anomaly") bool CanStartAnomaly(const APOSTAnomaly* Anomaly) const;
    void NotifyAnomalyStarted(APOSTAnomaly* Anomaly);
    void NotifyAnomalyStopped(APOSTAnomaly* Anomaly);

    // Legacy Presence API is intentionally non-lethal. Remove old PresenceTrigger actors from maps after migration.
    UPROPERTY(BlueprintAssignable, Category="POST|Presence|Legacy") FPOSTPresenceStateChanged OnPresenceStateChanged;
    UPROPERTY(VisibleAnywhere, Category="POST|Presence|Legacy") EPOSTPresenceState PresenceState = EPOSTPresenceState::Inactive;
    UFUNCTION(BlueprintCallable, Category="POST|Presence|Legacy") void StartPresenceEncounter();
    UFUNCTION(BlueprintCallable, Category="POST|Presence|Legacy") void StopPresenceEncounter();
    UFUNCTION(BlueprintPure, Category="POST|Presence|Legacy") EPOSTPresenceState GetPresenceState() const { return PresenceState; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Radio") TArray<FPOSTRadioMessage> RadioMessages;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly", meta=(ClampMin="0.0")) float GlobalAnomalyCooldown = 5.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly", meta=(ClampMin="1")) int32 MaxConcurrentAnomalies = 1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") EPOSTStoryStage StoryStage = EPOSTStoryStage::Arrival;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") EPOSTDeathCause LastDeathCause = EPOSTDeathCause::Unknown;

private:
    void CacheWorldReferences();
    void SetPresenceState(EPOSTPresenceState NewState);

    UPROPERTY(Transient) APOSTCharacter* Player = nullptr;
    UPROPERTY(Transient) APOSTCycleManager* CycleManager = nullptr;
    UPROPERTY(Transient) TArray<APOSTAnomaly*> Anomalies;
    UPROPERTY(Transient) TArray<APOSTAnomaly*> ActiveAnomalies;
    UPROPERTY(Transient) TSet<FName> PlayedRadioMessages;

    float LastAnomalyFinishedWorldTime = -1.0f;
};
