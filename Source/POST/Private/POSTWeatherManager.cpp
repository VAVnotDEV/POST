// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "POSTWeatherManager.h"
#include "POSTGameState.h"
#include "Kismet/GameplayStatics.h"
#include "POSTLog.h"
#include "TimerManager.h"

// Sets default values
APOSTWeatherManager::APOSTWeatherManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APOSTWeatherManager::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APOSTWeatherManager::UpdateOutdoorTemperature, 2.0f, false);
	
}

// Called every frame
void APOSTWeatherManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float APOSTWeatherManager::GetOutdoorTemperature() const
{
	
	const APOSTGameState* POSTGameState = Cast<APOSTGameState>(UGameplayStatics::GetGameState(GetWorld()));
	
	float result = BaseTemperature;
	
	if (!POSTGameState) { return result; }

	if (POSTGameState->IsNight())
		result = BaseTemperature + NightModifier;

	else
		result = BaseTemperature + DayModifier;



	return result;
}

void APOSTWeatherManager::UpdateOutdoorTemperature()
{
	float temp = GetOutdoorTemperature();
	UE_LOG(LogPOST, Display, TEXT("WM: %f"), temp)
		OnTemperatureChanged.Broadcast(temp);
}

