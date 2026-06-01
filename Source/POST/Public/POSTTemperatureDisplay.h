// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTTemperatureDisplay.generated.h"

class UTextRenderComponent;
class UStaticMeshComponent;
class APOSTWeatherManager;

UCLASS()
class POST_API APOSTTemperatureDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOSTTemperatureDisplay();

	UFUNCTION()
	void HandleTemperatureChanged(float NewTemperature);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* TemperatureTextComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* DisplayMeshComponent;

	UPROPERTY()
	APOSTWeatherManager* WeatherManager;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
