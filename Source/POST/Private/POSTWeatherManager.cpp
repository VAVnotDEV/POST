// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#include "POSTWeatherManager.h"
#include "POSTGameState.h"
#include "Kismet/GameplayStatics.h"
#include "POSTLog.h"

APOSTWeatherManager::APOSTWeatherManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APOSTWeatherManager::BeginPlay()
{
    Super::BeginPlay();

    CachedGameState = Cast<APOSTGameState>(UGameplayStatics::GetGameState(this));
    if (CachedGameState)
    {
        CachedGameState->OnHourChanged.AddDynamic(this, &APOSTWeatherManager::HandleHourChanged);
        CachedGameState->OnNightStarted.AddDynamic(this, &APOSTWeatherManager::HandleDayNightChanged);
        CachedGameState->OnDayStarted.AddDynamic(this, &APOSTWeatherManager::HandleDayNightChanged);
    }

    UpdateOutdoorTemperature();
}

void APOSTWeatherManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (CachedGameState)
    {
        CachedGameState->OnHourChanged.RemoveDynamic(this, &APOSTWeatherManager::HandleHourChanged);
        CachedGameState->OnNightStarted.RemoveDynamic(this, &APOSTWeatherManager::HandleDayNightChanged);
        CachedGameState->OnDayStarted.RemoveDynamic(this, &APOSTWeatherManager::HandleDayNightChanged);
    }

    CachedGameState = nullptr;
    Super::EndPlay(EndPlayReason);
}

float APOSTWeatherManager::GetOutdoorTemperature() const
{
    const APOSTGameState* GameState = CachedGameState;
    if (!GameState)
    {
        GameState = Cast<APOSTGameState>(UGameplayStatics::GetGameState(this));
    }

    const float TimeModifier = GameState && GameState->IsNight() ? NightModifier : DayModifier;
    return BaseTemperature + TimeModifier + WeatherModifier;
}

void APOSTWeatherManager::UpdateOutdoorTemperature()
{
    const float NewTemperature = GetOutdoorTemperature();
    if (FMath::IsNearlyEqual(NewTemperature, CachedOutdoorTemperature))
    {
        return;
    }

    CachedOutdoorTemperature = NewTemperature;
    UE_LOG(LogPOST, Display, TEXT("Weather temperature changed: %.1f C"), NewTemperature);
    OnTemperatureChanged.Broadcast(NewTemperature);
}

void APOSTWeatherManager::SetWeatherModifier(float NewModifier)
{
    if (FMath::IsNearlyEqual(WeatherModifier, NewModifier))
    {
        return;
    }

    WeatherModifier = NewModifier;
    UpdateOutdoorTemperature();
}

void APOSTWeatherManager::HandleHourChanged(int32 NewHour)
{
    UpdateOutdoorTemperature();
}

void APOSTWeatherManager::HandleDayNightChanged()
{
    UpdateOutdoorTemperature();
}
