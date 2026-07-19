// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlashLightItem.generated.h"

class USoundBase;
class UStaticMeshComponent;
class USpotLightComponent;

UCLASS(Blueprintable)
class POST_API AFlashLightItem : public AActor
{
    GENERATED_BODY()

public:
    AFlashLightItem();

    UFUNCTION(BlueprintCallable, Category="POST|Flashlight")
    void ToggleFlashLight();

    UFUNCTION(BlueprintCallable, Category="POST|Flashlight")
    void SetFlashLightEnabled(bool bEnabled, bool bPlaySound = false);

    UFUNCTION(BlueprintPure, Category="POST|Flashlight")
    bool IsOn() const { return bIsFlashlightOn; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Flashlight")
    UStaticMeshComponent* FlashlightStaticMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Flashlight")
    USpotLightComponent* LightComponent = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Flashlight|Sound")
    USoundBase* ToggleSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Flashlight")
    bool bStartEnabled = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Flashlight")
    bool bIsFlashlightOn = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Flashlight", meta=(ClampMin="0.0"))
    float MaxIntensity = 300.0f;
};
