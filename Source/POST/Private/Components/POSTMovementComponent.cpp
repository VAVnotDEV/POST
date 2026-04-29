// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/POSTMovementComponent.h"
#include "Player/POSTCharacter.h"

float UPOSTMovementComponent::GetMaxSpeed() const
{
	
	const float MaxSpeed = Super::GetMaxSpeed();
	UE_LOG(LogTemp, Warning, TEXT("Max Speed: %f"), MaxSpeed)
	const APOSTCharacter* Player = Cast<APOSTCharacter>(GetPawnOwner());
	return Player && Player->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}

