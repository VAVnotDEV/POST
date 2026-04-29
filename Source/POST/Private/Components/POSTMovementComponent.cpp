// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/POSTMovementComponent.h"
#include "Player/POSTCharacter.h"

float UPOSTMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();
	const APOSTCharacter* Player = Cast<APOSTCharacter>(GetPawnOwner());
	return Player && Player->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}

