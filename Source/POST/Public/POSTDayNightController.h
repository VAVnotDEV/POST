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

	void Night();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeOfDay")
	bool bIsNight = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	AActor* SkySphereActor;

	UPROPERTY(EditAnywhere, Category = "Components")
	ASkyLight* SkyLight;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	ADirectionalLight* DirectionalLight;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	APostProcessVolume* PostProcess;

	UPROPERTY(EditAnywhere, Category = "Components")
	AExponentialHeightFog* HeightFog;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Light")
	FRotator SunPosition = FRotator(-45.0f, 0.0f, 0.0f);
	
	UPROPERTY(EditAnywhere, Category = "Light")
	float Exposure = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Light")
	float DirectionLightIntensity = 5.0f;
	
	UPROPERTY(EditAnywhere, Category = "Light")
	float SkyLightIntensity = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Light")
	bool HeightFogVisibility = true;

	UPROPERTY(EditAnywhere, Category = "Light")
	FLinearColor FogColor = FLinearColor::White;
};
