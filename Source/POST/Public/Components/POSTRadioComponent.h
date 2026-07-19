#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTRadioComponent.generated.h"

class UAudioComponent;
class USoundBase;

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

    UPROPERTY(BlueprintAssignable, Category="POST|Radio")
    FPOSTRadioMessageFinished OnMessageFinished;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POST|Radio")
    USoundBase* InterferenceLoop = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POST|Radio", meta=(ClampMin="0.0"))
    float MaximumInterferenceVolume = 0.8f;

private:
    UFUNCTION()
    void HandleAudioFinished();

    UPROPERTY(Transient)
    UAudioComponent* AudioComponent = nullptr;

    FName CurrentMessageId = NAME_None;
    bool bMessagePlaying = false;
};
