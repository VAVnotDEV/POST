// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTWeatherManager.generated.h"

class APOSTGameState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTemperatureChanged, float);

UCLASS(Blueprintable)
class POST_API APOSTWeatherManager : public AActor
{
    GENERATED_BODY()

public:
    APOSTWeatherManager();

    FOnTemperatureChanged OnTemperatureChanged;

    UFUNCTION(BlueprintPure, Category="POST|Weather")
    float GetOutdoorTemperature() const;

    UFUNCTION(BlueprintCallable, Category="POST|Weather")
    void UpdateOutdoorTemperature();

    UFUNCTION(BlueprintCallable, Category="POST|Weather")
    void SetWeatherModifier(float NewModifier);

    UFUNCTION(BlueprintPure, Category="POST|Weather")
    float GetWeatherModifier() const { return WeatherModifier; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Weather")
    float BaseTemperature = -25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Weather")
    float DayModifier = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Weather")
    float NightModifier = -10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Weather")
    float WeatherModifier = 0.0f;

private:
    UPROPERTY()
    APOSTGameState* CachedGameState = nullptr;

    float CachedOutdoorTemperature = TNumericLimits<float>::Lowest();

    UFUNCTION()
    void HandleHourChanged(int32 NewHour);

    UFUNCTION()
    void HandleDayNightChanged();
};
