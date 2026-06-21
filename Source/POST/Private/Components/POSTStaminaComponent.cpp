// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "Components/POSTStaminaComponent.h"
#include "POSTLog.h"

// Sets default values for this component's properties
UPOSTStaminaComponent::UPOSTStaminaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	

	// ...
}

bool UPOSTStaminaComponent::HasStamina() const
{
	return CurrentStamina > 0.0f;
}

bool UPOSTStaminaComponent::CanRun() const
{
	return CurrentStamina >= MinStaminaToRun;
}

void UPOSTStaminaComponent::StartSpendStamina()
{
	if (CanRun())
		bIsSpendingStamina = true;
}

void UPOSTStaminaComponent::StopSpendStamina()
{
	bIsSpendingStamina = false;
}

float UPOSTStaminaComponent::GetCurrentStamina() const
{
	return CurrentStamina;
}


// Called when the game starts
void UPOSTStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
	
}

// Called every frame
void UPOSTStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	StaminaUpdate();
}

void UPOSTStaminaComponent::SetStamina(float NewStamina)
{
	CurrentStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina);
}

void UPOSTStaminaComponent::StaminaUpdate()
{
	if (bIsSpendingStamina && HasStamina())
		SetStamina(CurrentStamina -= StaminaSpendRate);
	else
		SetStamina(CurrentStamina += StaminaRegenRate);
}

