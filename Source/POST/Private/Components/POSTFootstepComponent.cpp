// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "Components/POSTFootstepComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/POSTCharacter.h"
#include "Sound/SoundBase.h"

// Sets default values for this component's properties
UPOSTFootstepComponent::UPOSTFootstepComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPOSTFootstepComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(FootstepTimerHandle, this, &UPOSTFootstepComponent::PlayFootstep, WalkStepInterval, true);
	// ...
	
}


// Called every frame
void UPOSTFootstepComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPOSTFootstepComponent::PlayFootstep()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
    if (!Movement) return;

    const float Speed = Character->GetVelocity().Size2D();

    if (Speed < MinSpeedToPlay) return;
    if (!Movement->IsMovingOnGround()) return;

    if (SnowFootstepSounds.Num() > 0)
    {
        const int32 RandomIndex = FMath::RandRange(0, SnowFootstepSounds.Num() - 1);

        USoundBase* SelectedSound = SnowFootstepSounds[RandomIndex];

        if (SelectedSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                SelectedSound,
                Character->GetActorLocation()
            );
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        FootstepTimerHandle,
        this,
        &UPOSTFootstepComponent::PlayFootstep,
        GetCurrentStepInterval(),
        true
    );
}

float UPOSTFootstepComponent::GetCurrentStepInterval()
{
    const APOSTCharacter* Character = Cast<APOSTCharacter>(GetOwner());

    if (Character && Character->IsRunning())
    {
        return RunStepInterval;
    }

    return WalkStepInterval;
}

