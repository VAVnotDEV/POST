// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#include "Components/POSTFootstepComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/POSTCharacter.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

UPOSTFootstepComponent::UPOSTFootstepComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPOSTFootstepComponent::BeginPlay()
{
    Super::BeginPlay();
    ScheduleNextFootstep(WalkStepInterval);
}

void UPOSTFootstepComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(FootstepTimerHandle);
    }

    Super::EndPlay(EndPlayReason);
}

void UPOSTFootstepComponent::SetFootstepsEnabled(bool bEnabled)
{
    bFootstepsEnabled = bEnabled;

    if (!GetWorld())
    {
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(FootstepTimerHandle);
    if (bFootstepsEnabled)
    {
        ScheduleNextFootstep(GetCurrentStepInterval());
    }
}

void UPOSTFootstepComponent::ScheduleNextFootstep(float Delay)
{
    if (!GetWorld() || !bFootstepsEnabled)
    {
        return;
    }

    GetWorld()->GetTimerManager().SetTimer(
        FootstepTimerHandle,
        this,
        &UPOSTFootstepComponent::TryPlayFootstep,
        FMath::Max(0.05f, Delay),
        false);
}

void UPOSTFootstepComponent::TryPlayFootstep()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        ScheduleNextFootstep(WalkStepInterval);
        return;
    }

    UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
    if (!Movement)
    {
        ScheduleNextFootstep(WalkStepInterval);
        return;
    }

    const float Speed = Character->GetVelocity().Size2D();
    if (Speed >= MinSpeedToPlay && Movement->IsMovingOnGround() && SnowFootstepSounds.Num() > 0)
    {
        const int32 RandomIndex = FMath::RandRange(0, SnowFootstepSounds.Num() - 1);
        if (USoundBase* SelectedSound = SnowFootstepSounds[RandomIndex])
        {
            UGameplayStatics::PlaySoundAtLocation(this, SelectedSound, Character->GetActorLocation());
        }
    }

    ScheduleNextFootstep(GetCurrentStepInterval());
}

float UPOSTFootstepComponent::GetCurrentStepInterval() const
{
    const APOSTCharacter* Character = Cast<APOSTCharacter>(GetOwner());
    return Character && Character->IsRunning() ? RunStepInterval : WalkStepInterval;
}
