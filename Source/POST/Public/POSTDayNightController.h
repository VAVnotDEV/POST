// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTDayNightController.generated.h"

class ASkyLight;
class ADirectionalLight;
class APostProcessVolume;
class AExponentialHeightFog;
class APOSTGameState;

UCLASS(Blueprintable)
class POST_API APOSTDayNightController : public AActor
{
    GENERATED_BODY()

public:
    APOSTDayNightController();

    UFUNCTION(BlueprintCallable, Category="POST|TimeOfDay")
    void ApplyCurrentState();

    UFUNCTION(BlueprintCallable, Category="POST|TimeOfDay")
    void SetNight(bool bNewNight);

    UFUNCTION(BlueprintPure, Category="POST|TimeOfDay")
    bool IsNight() const { return bIsNight; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="POST|TimeOfDay")
    bool bIsNight = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|TimeOfDay")
    bool bFollowGameTime = true;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="POST|References")
    AActor* SkySphereActor = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="POST|References")
    ASkyLight* SkyLight = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="POST|References")
    ADirectionalLight* DirectionalLight = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="POST|References")
    APostProcessVolume* PostProcess = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="POST|References")
    AExponentialHeightFog* HeightFog = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Day")
    FRotator DaySunRotation = FRotator(-45.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Day")
    float DayExposure = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Day", meta=(ClampMin="0.0"))
    float DayDirectionalLightIntensity = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Day", meta=(ClampMin="0.0"))
    float DaySkyLightIntensity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Day")
    bool bDayFogVisible = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Day")
    FLinearColor DayFogColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Night")
    FRotator NightSunRotation = FRotator(90.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Night")
    float NightExposure = -1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Night", meta=(ClampMin="0.0"))
    float NightDirectionalLightIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Night", meta=(ClampMin="0.0"))
    float NightSkyLightIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Night")
    bool bNightFogVisible = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Night")
    FLinearColor NightFogColor = FLinearColor(0.02f, 0.03f, 0.05f);

private:
    UPROPERTY()
    APOSTGameState* CachedGameState = nullptr;

    UFUNCTION()
    void HandleNightStarted();

    UFUNCTION()
    void HandleDayStarted();

    void RefreshSkySphereMaterial() const;
};
