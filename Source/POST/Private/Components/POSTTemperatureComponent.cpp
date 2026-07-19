#include "Components/POSTTemperatureComponent.h"

UPOSTTemperatureComponent::UPOSTTemperatureComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPOSTTemperatureComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentTemperature = MaxTemperature;
    OnBodyTemperatureChanged.Broadcast(CurrentTemperature);
}

void UPOSTTemperatureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    const float Rate = WarmZoneCount > 0 ? HeatingPerSecond : -CoolingPerSecond;
    SetTemperature(CurrentTemperature + Rate * DeltaTime);
}

void UPOSTTemperatureComponent::SetInWarmZone(bool bIsInZone)
{
    WarmZoneCount = bIsInZone ? 1 : 0;
}

void UPOSTTemperatureComponent::AddWarmZone()
{
    ++WarmZoneCount;
}

void UPOSTTemperatureComponent::RemoveWarmZone()
{
    WarmZoneCount = FMath::Max(0, WarmZoneCount - 1);
}

void UPOSTTemperatureComponent::AddTemperature(float Amount)
{
    SetTemperature(CurrentTemperature + Amount);
}

void UPOSTTemperatureComponent::SetTemperature(float NewTemperature)
{
    const float Clamped = FMath::Clamp(NewTemperature, 0.0f, MaxTemperature);
    if (!FMath::IsNearlyEqual(CurrentTemperature, Clamped))
    {
        CurrentTemperature = Clamped;
        OnBodyTemperatureChanged.Broadcast(CurrentTemperature);
    }

    if (CurrentTemperature <= KINDA_SMALL_NUMBER && !bFrozenEventSent)
    {
        bFrozenEventSent = true;
        OnPlayerFrozen.Broadcast();
    }
    else if (CurrentTemperature > 5.0f)
    {
        bFrozenEventSent = false;
    }
}
