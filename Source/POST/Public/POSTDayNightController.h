// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTDayNightController.generated.h"

class ASkyLight;
class ADirectionalLight;
class APostProcessVolume;
class AExponentialHeightFog;

UCLASS()
class POST_API APOSTDayNightController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOSTDayNightController();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	AActor* SkySphereActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsNight = false;

	void Day();
	void Night();

protected:
	
	UPROPERTY(EditAnywhere, Category = "Light")
	ASkyLight* SkyLight;
	
	UPROPERTY(EditAnywhere, Category = "Light")
	ADirectionalLight* DirectionalLight;
	
	UPROPERTY(EditAnywhere, Category = "Light")
	APostProcessVolume* PostProcess;

	UPROPERTY(EditAnywhere, Category = "Light")
	AExponentialHeightFog* HeightFog;



	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FRotator SunDayPosition = FRotator(90.0f, 0.0f, 0.0f);
	FRotator SunNightPosition = FRotator(-45.0f, 0.0f, 0.0f);
	
	float ExposureDay = 1.0f;
	float ExposureNight = -1.0f;
	
	float LightDayIntensity = 1.0f;
	float LightNightIntensity = 0.0f;

};
