// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "Components/POSTTemperatureComponent.h"
#include "POSTLog.h"

// Sets default values for this component's properties
UPOSTTemperatureComponent::UPOSTTemperatureComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPOSTTemperatureComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentTemperature = MaxTemperature;
	
}

void UPOSTTemperatureComponent::SetTemperature(float NewTemp)
{

	CurrentTemperature = FMath::Clamp(NewTemp, 0.0f, MaxTemperature);
	OnBodyTemperatureChanged.Broadcast(CurrentTemperature);
}

void UPOSTTemperatureComponent::TemperatureUpdate()
{
	UE_LOG(LogPOST, Display, TEXT("Call TempUpdate: %f"), CurrentTemperature)
	if (bIsInWarmZone)
		SetTemperature(CurrentTemperature + HeatingRate);

	else
		SetTemperature(CurrentTemperature - CoolingRate);
}


// Called every frame
void UPOSTTemperatureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TemperatureUpdate();
}

void UPOSTTemperatureComponent::SetInWarmZone(bool bIsZone)
{
	bIsInWarmZone = bIsZone;
}

float UPOSTTemperatureComponent::GetCurrentTemperature()
{
	return CurrentTemperature;
}

