#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTGameDirector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOSTStoryStageChanged, EPOSTStoryStage, OldStage, EPOSTStoryStage, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOSTRebooted, int32, RebootCount, EPOSTDeathCause, Cause);

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
    UFUNCTION(BlueprintPure, Category="POST|Director") int32 GetRebootCount() const { return RebootCount; }
    UFUNCTION(BlueprintPure, Category="POST|Director") float GetWorldResourceMultiplier() const { return WorldResourceMultiplier; }
    UFUNCTION(BlueprintPure, Category="POST|Director") float GetGeneratorReliabilityMultiplier() const { return GeneratorReliabilityMultiplier; }
    UFUNCTION(BlueprintPure, Category="POST|Director") EPOSTDeathCause GetLastDeathCause() const { return LastDeathCause; }

    UFUNCTION(BlueprintCallable, Category="POST|Director") bool SetStoryStage(EPOSTStoryStage NewStage);
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool AdvanceStoryStage(EPOSTStoryStage ExpectedCurrentStage, EPOSTStoryStage NewStage);
    UFUNCTION(BlueprintCallable, Category="POST|Director") void RegisterDeath(EPOSTDeathCause Cause);
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool SaveProgress();
    UFUNCTION(BlueprintCallable, Category="POST|Director") bool LoadProgress();
    UFUNCTION(BlueprintCallable, Category="POST|Director") void ResetProgress();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Save") FString SaveSlotName = TEXT("POST_Autosave");
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Save") int32 SaveUserIndex = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot", meta=(ClampMin="0.1", ClampMax="1.0")) float ResourceLossPerReboot = 0.08f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot", meta=(ClampMin="0.1", ClampMax="1.0")) float ReliabilityLossPerReboot = 0.05f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot") float MinimumResourceMultiplier = 0.35f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Reboot") float MinimumReliabilityMultiplier = 0.45f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") EPOSTStoryStage StoryStage = EPOSTStoryStage::Arrival;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") int32 RebootCount = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") float WorldResourceMultiplier = 1.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") float GeneratorReliabilityMultiplier = 1.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|State") EPOSTDeathCause LastDeathCause = EPOSTDeathCause::Unknown;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Director") void OnColdAftereffectRequested();
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Director") void OnWorldRebootRequested(EPOSTDeathCause Cause);
};
