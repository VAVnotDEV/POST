#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTRadioComponent.generated.h"

class UAudioComponent;
class USoundBase;
class UPOSTEntityProximityComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTRadioMessageFinished, FName, MessageId);

UCLASS(ClassGroup=(POST), meta=(BlueprintSpawnableComponent))
class POST_API UPOSTRadioComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPOSTRadioComponent();

    UFUNCTION(BlueprintCallable, Category="POST|Radio")
    bool PlayMessage(FName MessageId, USoundBase* Sound);

    UFUNCTION(BlueprintCallable, Category="POST|Radio")
    void SetInterference(float NormalizedStrength);

    UFUNCTION(BlueprintCallable, Category="POST|Radio")
    void StopRadio();

    UFUNCTION(BlueprintPure, Category="POST|Radio")
    bool IsMessagePlaying() const { return bMessagePlaying; }

    UFUNCTION(BlueprintPure, Category="POST|Radio")
    float GetInterferenceStrength() const { return InterferenceStrength; }

    UPROPERTY(BlueprintAssignable, Category="POST|Radio")
    FPOSTRadioMessageFinished OnMessageFinished;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="POST|Radio")
    USoundBase* InterferenceLoop = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="POST|Radio", meta=(ClampMin="0.0"))
    float MaximumInterferenceVolume = 0.8f;

    // Below this proximity the radio is mostly quiet and only gives occasional bursts.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="POST|Radio|Entity", meta=(ClampMin="0.0", ClampMax="1.0"))
    float ContinuousInterferenceThreshold = 0.55f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="POST|Radio|Entity", meta=(ClampMin="0.1"))
    float FarBurstMinInterval = 3.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="POST|Radio|Entity", meta=(ClampMin="0.1"))
    float FarBurstMaxInterval = 7.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="POST|Radio|Entity", meta=(ClampMin="0.01"))
    float BurstDuration = 0.18f;

private:
    UFUNCTION()
    void HandleAudioFinished();

    void ApplyInterference();
    void StopAudioSilently();
    void UpdateEntityInterference(float DeltaTime);
    float ShapeContinuousInterference(float Proximity) const;

    UPROPERTY(Transient)
    UAudioComponent* AudioComponent = nullptr;

    UPROPERTY(Transient)
    UPOSTEntityProximityComponent* EntityProximity = nullptr;

    FName CurrentMessageId = NAME_None;
    float InterferenceStrength = 0.0f;
    float BurstCooldown = 0.0f;
    float BurstTimeRemaining = 0.0f;
    bool bMessagePlaying = false;
    bool bSuppressFinishedCallback = false;
};
