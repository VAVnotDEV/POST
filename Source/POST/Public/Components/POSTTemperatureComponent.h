// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTTemperatureComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POST_API UPOSTTemperatureComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPOSTTemperatureComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetInWarmZone(bool bIsZone);
	float GetCurrentTemperature();

private:
	float CurrentTemperature;
	float MaxTemperature = 100.0f;
	float MinTemterature = 0.0f;
	float CoolingRate = 3.0f;
	float HeatingRate = 5.0f;
	bool bIsInWarmZone = false;
};
