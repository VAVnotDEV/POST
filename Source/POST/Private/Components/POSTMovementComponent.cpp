// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "Components/POSTMovementComponent.h"
#include "Player/POSTCharacter.h"
#include "Components/POSTStaminaComponent.h"

float UPOSTMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();
	const APOSTCharacter* Player = Cast<APOSTCharacter>(GetPawnOwner());
	if (!Player) return MaxSpeed;

	return Player->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}

