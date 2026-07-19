#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTAnomaly.generated.h"

class APOSTGameDirector;

UCLASS(Blueprintable)
class POST_API APOSTAnomaly : public AActor
{
    GENERATED_BODY()

public:
    APOSTAnomaly();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category="POST|Anomaly") bool ActivateAnomaly();
    UFUNCTION(BlueprintCallable, Category="POST|Anomaly") void StopAnomaly();
    UFUNCTION(BlueprintPure, Category="POST|Anomaly") bool CanActivate() const;
    UFUNCTION(BlueprintPure, Category="POST|Anomaly") bool IsActive() const { return bActive; }
    UFUNCTION(BlueprintPure, Category="POST|Anomaly") bool BlocksOtherAnomalies() const { return bBlocksOtherAnomalies; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="POST|Anomaly") APOSTGameDirector* Director;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly") EPOSTStoryStage MinimumStage = EPOSTStoryStage::DrillingAnomaly;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly") bool bOneShot = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly") bool bBlocksOtherAnomalies = true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly", meta=(ClampMin="0.0")) float Cooldown = 15.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly", meta=(ClampMin="0.0")) float AutoStopDelay = 0.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Anomaly") bool bActive = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Anomaly") bool bWasActivated = false;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Anomaly") void OnAnomalyStarted();
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Anomaly") void OnAnomalyStopped();

private:
    void ResolveDirector();

    FTimerHandle StopTimer;
    float LastStoppedWorldTime = -1.0f;
};
