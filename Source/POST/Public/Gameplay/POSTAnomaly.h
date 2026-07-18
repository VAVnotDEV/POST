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
    UFUNCTION(BlueprintCallable, Category="POST|Anomaly") bool ActivateAnomaly();
    UFUNCTION(BlueprintCallable, Category="POST|Anomaly") void StopAnomaly();
    UFUNCTION(BlueprintPure, Category="POST|Anomaly") bool IsActive() const { return bActive; }
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="POST|Anomaly") APOSTGameDirector* Director;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly") EPOSTStoryStage MinimumStage = EPOSTStoryStage::DrillingAnomaly;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly") bool bOneShot = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Anomaly") float AutoStopDelay = 0.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Anomaly") bool bActive = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Anomaly") bool bWasActivated = false;
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Anomaly") void OnAnomalyStarted();
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Anomaly") void OnAnomalyStopped();
private:
    FTimerHandle StopTimer;
};
