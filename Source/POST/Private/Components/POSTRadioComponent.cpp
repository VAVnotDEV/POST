#include "Components/POSTRadioComponent.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

UPOSTRadioComponent::UPOSTRadioComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPOSTRadioComponent::BeginPlay()
{
    Super::BeginPlay();

    AudioComponent = NewObject<UAudioComponent>(GetOwner(), TEXT("PortableRadioAudio"));
    if (AudioComponent)
    {
        AudioComponent->bAutoActivate = false;
        AudioComponent->bIsUISound = true;
        AudioComponent->RegisterComponent();
        AudioComponent->OnAudioFinished.AddDynamic(this, &UPOSTRadioComponent::HandleAudioFinished);
    }
}

bool UPOSTRadioComponent::PlayMessage(FName MessageId, USoundBase* Sound)
{
    if (!AudioComponent || !Sound)
    {
        return false;
    }

    AudioComponent->Stop();
    AudioComponent->SetSound(Sound);
    AudioComponent->SetVolumeMultiplier(1.0f);
    CurrentMessageId = MessageId;
    bMessagePlaying = true;
    AudioComponent->Play();
    return true;
}

void UPOSTRadioComponent::SetInterference(float NormalizedStrength)
{
    if (!AudioComponent || bMessagePlaying || !InterferenceLoop)
    {
        return;
    }

    const float Strength = FMath::Clamp(NormalizedStrength, 0.0f, 1.0f);
    if (Strength <= KINDA_SMALL_NUMBER)
    {
        AudioComponent->Stop();
        return;
    }

    AudioComponent->Stop();
    AudioComponent->SetSound(InterferenceLoop);
    AudioComponent->SetVolumeMultiplier(Strength * MaximumInterferenceVolume);
    if (!AudioComponent->IsPlaying())
    {
        AudioComponent->Play();
    }
}

void UPOSTRadioComponent::StopRadio()
{
    if (AudioComponent)
    {
        AudioComponent->Stop();
    }
    CurrentMessageId = NAME_None;
    bMessagePlaying = false;
}

void UPOSTRadioComponent::HandleAudioFinished()
{
    if (!bMessagePlaying)
    {
        return;
    }

    const FName FinishedMessage = CurrentMessageId;
    CurrentMessageId = NAME_None;
    bMessagePlaying = false;
    OnMessageFinished.Broadcast(FinishedMessage);
}
