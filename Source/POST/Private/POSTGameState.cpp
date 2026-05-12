// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "POSTGameState.h"

APOSTGameState::APOSTGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool APOSTGameState::IsNight() const
{
	return Hours >= 22 || Hours < 6;
}

void APOSTGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AccumulateRealSeconds += DeltaTime * TimeScale;

	while (AccumulateRealSeconds >= 1.0f)
	{
		AccumulateRealSeconds -= 1.0f;
		AddGameSecond();
	}
}

void APOSTGameState::BeginPlay()
{
	Super::BeginPlay();

	bIsNight = IsNight();
	CheckDayNightState();
}

void APOSTGameState::AddGameSecond()
{
	Seconds++;

	if (Seconds >= 60)
	{
		Seconds = 0;
		Minutes++;

		OnMinuteChanged.Broadcast(Minutes);
		OnTimeChanged.Broadcast(Hours, Minutes);
	}

	if (Minutes >= 60)
	{
		Minutes = 0;
		Hours++;

		if (Hours >= 24)
			Hours = 0;

		OnHourChanged.Broadcast(Hours);
		CheckDayNightState();
	}


}

void APOSTGameState::CheckDayNightState()
{
	const bool bNowNight = IsNight();

	if (bNowNight && !bIsNight)
	{
		bIsNight = true;
		OnNightStarted.Broadcast();
	}

	else if (!bNowNight && bIsNight)
	{
		bIsNight = false;
		OnDayStarted.Broadcast();
	}
}
