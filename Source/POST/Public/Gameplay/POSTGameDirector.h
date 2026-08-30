#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTGameDirector.generated.h"

class APOSTAnomaly;
class APOSTCharacter;
class USoundBase;


USTRUCT(BlueprintType)
struct FPOSTRadioMessage
{
    GENERATED_BODY()


    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName MessageId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Sound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EPOSTStoryStage MinimumStage = EPOSTStoryStage::Arrival;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bPlayOnce = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOSTStoryStageChanged, EPOSTStoryStage, OldStage, EPOSTStoryStage, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOSTRebooted, int32, RebootCount, EPOSTDeathCause, Cause);
DECALRE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTPresenceStateChanged, EPOSTPresenceState, NewState);

UCLASS(Blueprintable)
class POST_API APOSTGameDirector : public AActor
{
    GENERATED_BODY()

public:
    APOSTGameDirector();
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintAssignable, Category="POST|Director") FPOSTStoryStageChanged OnStoryStageChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Director") FPOSTRebooted OnRebooted;

    UPROPERTY(BlueprintAssignable, Category = "POST|Presence") FPOSTPresenceStateChanged OnPresenceCtateChanged;

    UPROPERTY(BlueprintCallable, Category = "POST|Presence") EPOSTPresenceState PresenceState = EPOSTPresenceState::Inactive;

    UFUNCTION(BlueprintPure, Category="POST|Director") EPOSTStoryStage GetStoryStage() const { return StoryStage; }
    UFUNCTION(BlueprintPure, Category="POST|Director") int32 GetRebootCount() const { return RebootCount; }

    UFUNCTION(BlueprintPure, Category="POST|Director") EPOSTDeathCause GetLastDeathCause() const { return LastDeathCause; }
  
    UFUNCTION(BlueprintPure, Category="POST|Director") bool IsRebootInProgress() const { return bRebootInProgress; }

    UFUNCTION(BlueprintCallable, Category="POST|Director") bool SetStoryStage(EPOSTStoryStage NewStage);
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool AdvanceStoryStage(EPOSTStoryStage ExpectedCurrentStage, EPOSTStoryStage NewStage);
    UFUNCTION(BlueprintCallable, Category="POST|Director") void RegisterDeath(EPOSTDeathCause Cause);
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool SaveProgress();
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool LoadProgress();
    UFUNCTION(BlueprintCallable, Category="POST|Director") void ResetProgress();

    UFUNCTION(BlueprintCallable, Category="POST|Director|Radio") bool PlayRadioMessage(FName MessageId);

    UFUNCTION(BlueprintCallable, Category="POST|Director|Anomaly") bool ActivateAnomalyByName(FName ActorName);
    UFUNCTION(BlueprintCallable, Category="POST|Director|Anomaly") bool TryActivateNearbyAnomaly();
    UFUNCTION(BlueprintPure, Category="POST|Director|Anomaly") bool CanStartAnomaly(const APOSTAnomaly* Anomaly) const;
    void NotifyAnomalyStarted(APOSTAnomaly* Anomaly);
    void NotifyAnomalyStopped(APOSTAnomaly* Anomaly);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Save") FString SaveSlotName = TEXT("POST_Autosave");
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Save") int32 SaveUserIndex = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot", meta=(ClampMin="0.0")) float RebootDelay = 1.5f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot") bool bReloadCurrentLevelOnDeath = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Radio") TArray<FPOSTRadioMessage> RadioMessages;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly", meta=(ClampMin="0.0")) float GlobalAnomalyCooldown = 5.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly", meta=(ClampMin="1")) int32 MaxConcurrentAnomalies = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") EPOSTStoryStage StoryStage = EPOSTStoryStage::Arrival;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") int32 RebootCount = 0;
  
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") EPOSTDeathCause LastDeathCause = EPOSTDeathCause::Unknown;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Director") void OnColdAftereffectRequested();
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Director") void OnWorldRebootRequested(EPOSTDeathCause Cause);

private:
    void CacheWorldReferences();
    void ExecuteWorldReboot();
    void ApplySavedWorldState();

    UPROPERTY(Transient) APOSTCharacter* Player = nullptr;
    UPROPERTY(Transient) TArray<APOSTAnomaly*> Anomalies;
    UPROPERTY(Transient) TArray<APOSTAnomaly*> ActiveAnomalies;
    UPROPERTY(Transient) TSet<FName> PlayedRadioMessages;



    FTimerHandle RebootTimer;
    bool bRebootInProgress = false;
    float LastAnomalyFinishedWorldTime = -1.0f;
    bool bHasSavedWorldTime = false;
    int32 SavedDay = 1;
    int32 SavedHours = 21;
    int32 SavedMinutes = 0;
    int32 SavedSeconds = 0;
};
