// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#include "POSTDayNightController.h"
#include "POSTGameState.h"
#include "Components/SkyLightComponent.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "POSTLog.h"

APOSTDayNightController::APOSTDayNightController()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APOSTDayNightController::BeginPlay()
{
    Super::BeginPlay();

    CachedGameState = Cast<APOSTGameState>(UGameplayStatics::GetGameState(this));
    if (CachedGameState && bFollowGameTime)
    {
        CachedGameState->OnNightStarted.AddDynamic(this, &APOSTDayNightController::HandleNightStarted);
        CachedGameState->OnDayStarted.AddDynamic(this, &APOSTDayNightController::HandleDayStarted);
        bIsNight = CachedGameState->IsNight();
    }

    ApplyCurrentState();
}

void APOSTDayNightController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (CachedGameState)
    {
        CachedGameState->OnNightStarted.RemoveDynamic(this, &APOSTDayNightController::HandleNightStarted);
        CachedGameState->OnDayStarted.RemoveDynamic(this, &APOSTDayNightController::HandleDayStarted);
    }

    CachedGameState = nullptr;
    Super::EndPlay(EndPlayReason);
}

void APOSTDayNightController::SetNight(bool bNewNight)
{
    if (bIsNight == bNewNight)
    {
        ApplyCurrentState();
        return;
    }

    bIsNight = bNewNight;
    ApplyCurrentState();
}

void APOSTDayNightController::ApplyCurrentState()
{
    const FRotator& SunRotation = bIsNight ? NightSunRotation : DaySunRotation;
    const float Exposure = bIsNight ? NightExposure : DayExposure;
    const float DirectionalIntensity = bIsNight ? NightDirectionalLightIntensity : DayDirectionalLightIntensity;
    const float SkyIntensity = bIsNight ? NightSkyLightIntensity : DaySkyLightIntensity;
    const bool bFogVisible = bIsNight ? bNightFogVisible : bDayFogVisible;
    const FLinearColor& FogColor = bIsNight ? NightFogColor : DayFogColor;

    if (PostProcess)
    {
        PostProcess->Settings.AutoExposureBias = Exposure;
    }

    if (DirectionalLight)
    {
        DirectionalLight->SetActorRotation(SunRotation);
        if (UDirectionalLightComponent* LightComponent = Cast<UDirectionalLightComponent>(DirectionalLight->GetLightComponent()))
        {
            LightComponent->SetIntensity(DirectionalIntensity);
        }
    }
    else
    {
        UE_LOG(LogPOST, Warning, TEXT("DayNightController '%s' has no DirectionalLight assigned"), *GetName());
    }

    if (SkyLight)
    {
        if (USkyLightComponent* LightComponent = SkyLight->GetLightComponent())
        {
            LightComponent->SetIntensity(SkyIntensity);
            LightComponent->RecaptureSky();
        }
    }

    if (HeightFog)
    {
        if (UExponentialHeightFogComponent* FogComponent = HeightFog->GetComponent())
        {
            FogComponent->SetVisibility(bFogVisible);
            FogComponent->SetFogInscatteringColor(FogColor);
        }
    }

    RefreshSkySphereMaterial();
}

void APOSTDayNightController::HandleNightStarted()
{
    SetNight(true);
}

void APOSTDayNightController::HandleDayStarted()
{
    SetNight(false);
}

void APOSTDayNightController::RefreshSkySphereMaterial() const
{
    if (!SkySphereActor)
    {
        return;
    }

    static const FName RefreshFunctionName(TEXT("RefreshMaterial"));
    if (UFunction* RefreshFunction = SkySphereActor->FindFunction(RefreshFunctionName))
    {
        SkySphereActor->ProcessEvent(RefreshFunction, nullptr);
    }
}
