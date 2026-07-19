#include "POSTGameState.h"

APOSTGameState::APOSTGameState()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APOSTGameState::BeginPlay()
{
    Super::BeginPlay();
    bWasNight = IsNight();
    OnTimeChanged.Broadcast(Hours, Minutes);
}

void APOSTGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (bTimePaused || TimeScale <= 0.0f) return;

    AccumulatedGameSeconds += DeltaTime * TimeScale;
    while (AccumulatedGameSeconds >= 1.0f)
    {
        AccumulatedGameSeconds -= 1.0f;
        AddGameSecond();
    }
}

bool APOSTGameState::IsNight() const
{
    return Hours >= NightStartHour || Hours < DayStartHour;
}

void APOSTGameState::SetGameTime(int32 NewDay, int32 NewHours, int32 NewMinutes, int32 NewSeconds)
{
    Day = FMath::Max(1, NewDay);
    Hours = FMath::Clamp(NewHours, 0, 23);
    Minutes = FMath::Clamp(NewMinutes, 0, 59);
    Seconds = FMath::Clamp(NewSeconds, 0, 59);
    AccumulatedGameSeconds = 0.0f;
    OnDayChanged.Broadcast(Day);
    OnHourChanged.Broadcast(Hours);
    OnMinuteChanged.Broadcast(Minutes);
    OnTimeChanged.Broadcast(Hours, Minutes);
    CheckDayNightState();
}

void APOSTGameState::AdvanceMinutes(int32 MinutesToAdvance)
{
    if (MinutesToAdvance <= 0) return;
    for (int32 Index = 0; Index < MinutesToAdvance * 60; ++Index) AddGameSecond();
}

void APOSTGameState::AddGameSecond()
{
    ++Seconds;
    if (Seconds < 60) return;

    Seconds = 0;
    ++Minutes;
    if (Minutes >= 60)
    {
        Minutes = 0;
        ++Hours;
        if (Hours >= 24)
        {
            Hours = 0;
            ++Day;
            OnDayChanged.Broadcast(Day);
        }
        OnHourChanged.Broadcast(Hours);
        CheckDayNightState();
    }

    OnMinuteChanged.Broadcast(Minutes);
    OnTimeChanged.Broadcast(Hours, Minutes);
}

void APOSTGameState::CheckDayNightState()
{
    const bool bNowNight = IsNight();
    if (bNowNight == bWasNight) return;
    bWasNight = bNowNight;
    if (bWasNight) OnNightStarted.Broadcast();
    else OnDayStarted.Broadcast();
}
