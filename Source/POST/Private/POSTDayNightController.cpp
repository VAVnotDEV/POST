// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "POSTDayNightController.h"
#include "Components/SkyLightComponent.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "POSTLog.h"

// Sets default values
APOSTDayNightController::APOSTDayNightController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkyLight = nullptr;
	DirectionalLight = nullptr;
	PostProcess = nullptr;
	HeightFog = nullptr;
	SkySphereActor = nullptr;
}

void APOSTDayNightController::Day()
{
	PostProcess->Settings.AutoExposureBias = ExposureDay;
	DirectionalLight->SetActorRotation(SunDayPosition);
	DirectionalLight->GetLightComponent()->SetIntensity(DirectionLightDayIntensity);
	SkyLight->GetLightComponent()->SetIntensity(SkyLightDayIntensity);
	HeightFog->GetComponent()->SetVisibility(true);
	UE_LOG(LogPOST, Display, TEXT(""))
}

void APOSTDayNightController::Night()
{
	PostProcess->Settings.AutoExposureBias = ExposureNight;
	DirectionalLight->SetActorRotation(SunNightPosition);
	DirectionalLight->GetLightComponent()->SetIntensity(LightNightIntensity);
	SkyLight->GetLightComponent()->SetIntensity(LightNightIntensity);
	HeightFog->GetComponent()->SetVisibility(false);
}

// Called when the game starts or when spawned
void APOSTDayNightController::BeginPlay()
{
	Super::BeginPlay();

	if (bIsNight)
		Night();
	else
		Day();
}

// Called every frame
void APOSTDayNightController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

