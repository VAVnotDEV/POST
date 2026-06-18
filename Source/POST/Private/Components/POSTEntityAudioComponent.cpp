// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "Components/POSTEntityAudioComponent.h"
#include "EntityMonster/POSTEntityPresence.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

// Sets default values for this component's properties
UPOSTEntityAudioComponent::UPOSTEntityAudioComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPOSTEntityAudioComponent::BeginPlay()
{
	Super::BeginPlay();

    EntityPresence = Cast<APOSTEntityPresence>(
        UGameplayStatics::GetActorOfClass(GetWorld(), APOSTEntityPresence::StaticClass())
    );

    if (PresenceLoopSound)
    {
        AudioComponent = UGameplayStatics::SpawnSound2D(
            GetWorld(),
            PresenceLoopSound,
            0.0f,
            1.0f,
            0.0f
        );
    }
	
}


// Called every frame
void UPOSTEntityAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!AudioComponent || !EntityPresence) return;

    const float Threat = EntityPresence->GetThreatLevel();

    const float Volume = FMath::Lerp(0.0f, 0.8f, Threat);
    const float Pitch = FMath::Lerp(0.85f, 1.25f, Threat);

    AudioComponent->SetVolumeMultiplier(Volume);
    AudioComponent->SetPitchMultiplier(Pitch);
}

