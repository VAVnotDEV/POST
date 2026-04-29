// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "POSTMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class POST_API UPOSTMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "1.5", ClampMax = "5.0"))
	float RunModifier = 2.0f;

	virtual float GetMaxSpeed() const override;
	
};
