// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTFootstepComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POST_API UPOSTFootstepComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPOSTFootstepComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category = "Sound")
	TArray<USoundBase*> SnowFootstepSounds;

	UPROPERTY(EditAnywhere, Category = "Footsteps")
	float WalkStepInterval = 0.55f;

	UPROPERTY(EditAnywhere, Category = "Footsteps")
	float RunStepInterval = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Footsteps")
	float MinSpeedToPlay = 10.0f;

	FTimerHandle FootstepTimerHandle;

	void PlayFootstep();
	float GetCurrentStepInterval();
};
