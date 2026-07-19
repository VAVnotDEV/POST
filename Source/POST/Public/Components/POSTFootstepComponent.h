// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTFootstepComponent.generated.h"

class USoundBase;

UCLASS(ClassGroup=(POST), meta=(BlueprintSpawnableComponent))
class POST_API UPOSTFootstepComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPOSTFootstepComponent();

    UFUNCTION(BlueprintCallable, Category="POST|Footsteps")
    void SetFootstepsEnabled(bool bEnabled);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(EditAnywhere, Category="POST|Footsteps")
    TArray<USoundBase*> SnowFootstepSounds;

    UPROPERTY(EditAnywhere, Category="POST|Footsteps", meta=(ClampMin="0.05"))
    float WalkStepInterval = 0.55f;

    UPROPERTY(EditAnywhere, Category="POST|Footsteps", meta=(ClampMin="0.05"))
    float RunStepInterval = 0.35f;

    UPROPERTY(EditAnywhere, Category="POST|Footsteps", meta=(ClampMin="0.0"))
    float MinSpeedToPlay = 10.0f;

    FTimerHandle FootstepTimerHandle;
    bool bFootstepsEnabled = true;

    void ScheduleNextFootstep(float Delay);
    void TryPlayFootstep();
    float GetCurrentStepInterval() const;
};
