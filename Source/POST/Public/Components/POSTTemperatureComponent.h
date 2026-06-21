// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTTemperatureComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBodyTemperaureChanged, float)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POST_API UPOSTTemperatureComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPOSTTemperatureComponent();

	FOnBodyTemperaureChanged OnBodyTemperatureChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetTemperature(float NewTemp);
	void TemperatureUpdate();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetInWarmZone(bool bIsZone);
	float GetCurrentTemperature();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Temperature")
	float CurrentTemperature;
	
	UPROPERTY(EditDefaultsOnly, Category = "Temperature")
	float MaxTemperature = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Temperature")
	float CoolingRate = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Temperature")
	float HeatingRate = 5.0f;

	bool bIsInWarmZone = false;
};
