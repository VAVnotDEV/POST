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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTThreatChanged, float, ThreatLevel);

UCLASS(Blueprintable)
class POST_API APOSTGameDirector : public AActor
{
    GENERATED_BODY()

public:
    APOSTGameDirector();
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintAssignable, Category="POST|Director") FPOSTStoryStageChanged OnStoryStageChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Director") FPOSTRebooted OnRebooted;
    UPROPERTY(BlueprintAssignable, Category="POST|Director") FPOSTThreatChanged OnThreatChanged;

    UFUNCTION(BlueprintPure, Category="POST|Director") EPOSTStoryStage GetStoryStage() const { return StoryStage; }
    UFUNCTION(BlueprintPure, Category="POST|Director") int32 GetRebootCount() const { return RebootCount; }
    UFUNCTION(BlueprintPure, Category="POST|Director") float GetWorldResourceMultiplier() const { return WorldResourceMultiplier; }
    UFUNCTION(BlueprintPure, Category="POST|Director") float GetGeneratorReliabilityMultiplier() const { return GeneratorReliabilityMultiplier; }
    UFUNCTION(BlueprintPure, Category="POST|Director") EPOSTDeathCause GetLastDeathCause() const { return LastDeathCause; }
    UFUNCTION(BlueprintPure, Category="POST|Director") float GetThreatLevel() const { return ThreatLevel; }

    UFUNCTION(BlueprintCallable, Category="POST|Director") bool SetStoryStage(EPOSTStoryStage NewStage);
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool AdvanceStoryStage(EPOSTStoryStage ExpectedCurrentStage, EPOSTStoryStage NewStage);
    UFUNCTION(BlueprintCallable, Category="POST|Director") void RegisterDeath(EPOSTDeathCause Cause);
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool SaveProgress();
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool LoadProgress();
    UFUNCTION(BlueprintCallable, Category="POST|Director") void ResetProgress();

    UFUNCTION(BlueprintCallable, Category="POST|Director|Radio") bool PlayRadioMessage(FName MessageId);
    UFUNCTION(BlueprintCallable, Category="POST|Director|Entity") void SetEntityLocation(FVector NewLocation);
    UFUNCTION(BlueprintCallable, Category="POST|Director|Anomaly") bool ActivateAnomalyByName(FName ActorName);
    UFUNCTION(BlueprintCallable, Category="POST|Director|Anomaly") bool TryActivateNearbyAnomaly();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Save") FString SaveSlotName = TEXT("POST_Autosave");
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Save") int32 SaveUserIndex = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot", meta=(ClampMin="0.0", ClampMax="1.0")) float ResourceLossPerReboot = 0.08f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot", meta=(ClampMin="0.0", ClampMax="1.0")) float ReliabilityLossPerReboot = 0.05f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot") float MinimumResourceMultiplier = 0.35f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot") float MinimumReliabilityMultiplier = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Radio") TArray<FPOSTRadioMessage> RadioMessages;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity", meta=(ClampMin="100.0")) float ThreatRadius = 3000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly", meta=(ClampMin="0.0", ClampMax="1.0")) float MinimumThreatForAnomaly = 0.4f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly", meta=(ClampMin="0.1")) float DirectorUpdateInterval = 0.25f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") EPOSTStoryStage StoryStage = EPOSTStoryStage::Arrival;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") int32 RebootCount = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") float WorldResourceMultiplier = 1.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") float GeneratorReliabilityMultiplier = 1.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") EPOSTDeathCause LastDeathCause = EPOSTDeathCause::Unknown;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") float ThreatLevel = 0.0f;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Director") void OnColdAftereffectRequested();
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Director") void OnWorldRebootRequested(EPOSTDeathCause Cause);

private:
    void CacheWorldReferences();
    void UpdateDirector();

    UPROPERTY(Transient) APOSTCharacter* Player = nullptr;
    UPROPERTY(Transient) TArray<APOSTAnomaly*> Anomalies;
    UPROPERTY(Transient) TSet<FName> PlayedRadioMessages;

    FVector EntityLocation = FVector::ZeroVector;
    FTimerHandle DirectorTimer;
};
