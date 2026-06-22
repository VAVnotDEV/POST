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

void APOSTDayNightController::Night()
{
	SunPosition = FRotator( 90.0f, 0.0f, 0.0f);
	Exposure = -1.0f;
	DirectionLightIntensity = 0;
	SkyLightIntensity = 0;
	//HeightFogVisibility = false;
}

// Called when the game starts or when spawned
void APOSTDayNightController::BeginPlay()
{

	
	if (bIsNight)
		Night();

	if (!PostProcess) return;
	PostProcess->Settings.AutoExposureBias = Exposure;

	if (!DirectionalLight) return;
	DirectionalLight->SetActorRotation(SunPosition);
	DirectionalLight->GetLightComponent()->SetIntensity(DirectionLightIntensity);
	
	if (!SkyLight) return;
	SkyLight->GetLightComponent()->SetIntensity(SkyLightIntensity);
	
	if (!HeightFog) return;
	HeightFog->GetComponent()->SetVisibility(HeightFogVisibility);
	HeightFog->GetComponent()->FogInscatteringColor = FogColor;

	Super::BeginPlay();
}

// Called every frame
void APOSTDayNightController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

