#include "Components/POSTStaminaComponent.h"

UPOSTStaminaComponent::UPOSTStaminaComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPOSTStaminaComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentStamina = MaxStamina;
    OnStaminaChanged.Broadcast(CurrentStamina);
}

void UPOSTStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    const float Delta = (bIsSpendingStamina && HasStamina()) ? -StaminaSpendPerSecond * DeltaTime : StaminaRegenPerSecond * DeltaTime;
    SetStamina(CurrentStamina + Delta);
    if (!HasStamina()) bIsSpendingStamina = false;
}

void UPOSTStaminaComponent::StartSpendStamina()
{
    if (CanRun()) bIsSpendingStamina = true;
}

void UPOSTStaminaComponent::StopSpendStamina()
{
    bIsSpendingStamina = false;
}

void UPOSTStaminaComponent::RestoreStamina(float Amount)
{
    SetStamina(CurrentStamina + FMath::Max(0.0f, Amount));
}

void UPOSTStaminaComponent::SetStamina(float NewStamina)
{
    const float Clamped = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
    if (FMath::IsNearlyEqual(CurrentStamina, Clamped)) return;
    CurrentStamina = Clamped;
    OnStaminaChanged.Broadcast(CurrentStamina);
}
