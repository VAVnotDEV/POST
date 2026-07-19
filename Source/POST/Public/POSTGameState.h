#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "POSTGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHourChanged, int32, NewHour);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinuteChanged, int32, NewMinute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeChanged, int32, Hours, int32, Minutes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChanged, int32, NewDay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDayStarted);

UCLASS()
class POST_API APOSTGameState : public AGameStateBase
{
    GENERATED_BODY()
public:
    APOSTGameState();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintAssignable, Category="POST|Time") FOnHourChanged OnHourChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Time") FOnMinuteChanged OnMinuteChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Time") FOnTimeChanged OnTimeChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Time") FOnDayChanged OnDayChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Time") FOnNightStarted OnNightStarted;
    UPROPERTY(BlueprintAssignable, Category="POST|Time") FOnDayStarted OnDayStarted;

    UFUNCTION(BlueprintPure, Category="POST|Time") int32 GetHours() const { return Hours; }
    UFUNCTION(BlueprintPure, Category="POST|Time") int32 GetMinutes() const { return Minutes; }
    UFUNCTION(BlueprintPure, Category="POST|Time") int32 GetSeconds() const { return Seconds; }
    UFUNCTION(BlueprintPure, Category="POST|Time") int32 GetDay() const { return Day; }
    UFUNCTION(BlueprintPure, Category="POST|Time") bool IsNight() const;

    UFUNCTION(BlueprintCallable, Category="POST|Time") void SetGameTime(int32 NewDay, int32 NewHours, int32 NewMinutes, int32 NewSeconds = 0);
    UFUNCTION(BlueprintCallable, Category="POST|Time") void AdvanceMinutes(int32 MinutesToAdvance);
    UFUNCTION(BlueprintCallable, Category="POST|Time") void SetTimePaused(bool bPaused) { bTimePaused = bPaused; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Time", meta=(ClampMin="1")) int32 Day = 1;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Time", meta=(ClampMin="0", ClampMax="23")) int32 Hours = 21;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Time", meta=(ClampMin="0", ClampMax="59")) int32 Minutes = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Time", meta=(ClampMin="0", ClampMax="59")) int32 Seconds = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Time", meta=(ClampMin="0.0")) float TimeScale = 60.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Time") int32 NightStartHour = 22;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Time") int32 DayStartHour = 6;

private:
    float AccumulatedGameSeconds = 0.0f;
    bool bWasNight = false;
    bool bTimePaused = false;
    void AddGameSecond();
    void CheckDayNightState();
};
