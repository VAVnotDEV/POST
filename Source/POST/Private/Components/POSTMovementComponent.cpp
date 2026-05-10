// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "Components/POSTMovementComponent.h"
#include "Player/POSTCharacter.h"
#include "Components/POSTStaminaComponent.h"

float UPOSTMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();
	const APOSTCharacter* Player = Cast<APOSTCharacter>(GetPawnOwner());
	if (!Player) return MaxSpeed;

	UPOSTStaminaComponent* Stamina = Player->GetStaminaComponent();
	if (!Stamina) return MaxSpeed;

	if (Player->IsRunning())
	{
		Stamina->StartSpendStamina();
		return MaxSpeed * RunModifier;
	}
	else
	{
		Stamina->StopSpendStamina();
		return MaxSpeed;
	}
	//return Player && Player->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}

