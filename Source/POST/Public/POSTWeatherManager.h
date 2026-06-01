// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTWeatherManager.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTemperatureChanged, float);

UCLASS()
class POST_API APOSTWeatherManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOSTWeatherManager();
	FOnTemperatureChanged OnTemperatureChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	float BaseTemperature = -25.0f;
	
	UPROPERTY(EditAnywhere)
	float DayModifier = 5.0f;
	
	UPROPERTY(EditAnywhere)
	float NightModifier = -10.0f;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float GetOutdoorTemperature() const;
};
