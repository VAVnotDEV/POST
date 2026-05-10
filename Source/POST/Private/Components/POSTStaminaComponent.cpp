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
	return CurrentStamina > MinStamina;
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

	if (bIsSpendingStamina && HasStamina())
		CurrentStamina -= StaminaSpendRate * DeltaTime;
	else
		CurrentStamina += StaminaRegenRate * DeltaTime;

	CurrentStamina = FMath::Clamp(CurrentStamina, MinStamina, MaxStamina);

	UE_LOG(LogPOST, Display, TEXT("Stamina: %f"), CurrentStamina)
}

