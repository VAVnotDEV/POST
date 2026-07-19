#include "Components/POSTRadioComponent.h"

#include "Components/AudioComponent.h"
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

void UPOSTRadioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopRadio();

    if (AudioComponent)
    {
        AudioComponent->OnAudioFinished.RemoveDynamic(this, &UPOSTRadioComponent::HandleAudioFinished);
        AudioComponent->DestroyComponent();
        AudioComponent = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

bool UPOSTRadioComponent::PlayMessage(FName MessageId, USoundBase* Sound)
{
    if (!AudioComponent || !Sound)
    {
        return false;
    }

    StopAudioSilently();
    AudioComponent->SetSound(Sound);
    AudioComponent->SetVolumeMultiplier(1.0f);
    CurrentMessageId = MessageId;
    bMessagePlaying = true;
    AudioComponent->Play();
    return true;
}

void UPOSTRadioComponent::SetInterference(float NormalizedStrength)
{
    InterferenceStrength = FMath::Clamp(NormalizedStrength, 0.0f, 1.0f);

    if (!bMessagePlaying)
    {
        ApplyInterference();
    }
}

void UPOSTRadioComponent::StopRadio()
{
    StopAudioSilently();
    CurrentMessageId = NAME_None;
    bMessagePlaying = false;
    InterferenceStrength = 0.0f;
}

void UPOSTRadioComponent::HandleAudioFinished()
{
    if (bSuppressFinishedCallback || !bMessagePlaying)
    {
        return;
    }

    const FName FinishedMessage = CurrentMessageId;
    CurrentMessageId = NAME_None;
    bMessagePlaying = false;
    OnMessageFinished.Broadcast(FinishedMessage);
    ApplyInterference();
}

void UPOSTRadioComponent::ApplyInterference()
{
    if (!AudioComponent || bMessagePlaying)
    {
        return;
    }

    if (!InterferenceLoop || InterferenceStrength <= KINDA_SMALL_NUMBER)
    {
        StopAudioSilently();
        return;
    }

    const bool bNeedsRestart = AudioComponent->Sound != InterferenceLoop || !AudioComponent->IsPlaying();
    if (bNeedsRestart)
    {
        StopAudioSilently();
        AudioComponent->SetSound(InterferenceLoop);
        AudioComponent->Play();
    }

    AudioComponent->SetVolumeMultiplier(InterferenceStrength * MaximumInterferenceVolume);
}

void UPOSTRadioComponent::StopAudioSilently()
{
    if (!AudioComponent || !AudioComponent->IsPlaying())
    {
        return;
    }

    bSuppressFinishedCallback = true;
    AudioComponent->Stop();
    bSuppressFinishedCallback = false;
}
