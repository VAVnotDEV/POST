#include "Components/POSTRadioComponent.h"

#include "Components/AudioComponent.h"
#include "Gameplay/POSTEntityProximityComponent.h"
#include "Sound/SoundBase.h"

UPOSTRadioComponent::UPOSTRadioComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
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

    EntityProximity = GetOwner() ? GetOwner()->FindComponentByClass<UPOSTEntityProximityComponent>() : nullptr;
    BurstCooldown = FMath::FRandRange(FarBurstMinInterval, FarBurstMaxInterval);
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

void UPOSTRadioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!EntityProximity && GetOwner())
    {
        EntityProximity = GetOwner()->FindComponentByClass<UPOSTEntityProximityComponent>();
    }

    UpdateEntityInterference(DeltaTime);
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
    BurstTimeRemaining = 0.0f;
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

void UPOSTRadioComponent::UpdateEntityInterference(float DeltaTime)
{
    if (!EntityProximity || bMessagePlaying)
    {
        return;
    }

    const float Proximity = EntityProximity->GetProximityIntensity();

    if (Proximity >= ContinuousInterferenceThreshold)
    {
        BurstTimeRemaining = 0.0f;
        BurstCooldown = 0.0f;
        SetInterference(ShapeContinuousInterference(Proximity));
        return;
    }

    // Far away: do not expose a clean distance meter. The radio only cracks
    // occasionally, with bursts becoming more frequent and stronger as the Entity approaches.
    if (Proximity <= KINDA_SMALL_NUMBER)
    {
        BurstTimeRemaining = 0.0f;
        SetInterference(0.0f);
        return;
    }

    if (BurstTimeRemaining > 0.0f)
    {
        BurstTimeRemaining -= DeltaTime;
        SetInterference(FMath::Clamp(0.12f + Proximity * 0.65f, 0.0f, 0.55f));
        return;
    }

    SetInterference(0.0f);
    BurstCooldown -= DeltaTime;

    if (BurstCooldown <= 0.0f)
    {
        BurstTimeRemaining = BurstDuration * FMath::FRandRange(0.7f, 1.4f);

        const float Nearness = FMath::Clamp(Proximity / FMath::Max(ContinuousInterferenceThreshold, 0.01f), 0.0f, 1.0f);
        const float BaseInterval = FMath::Lerp(FarBurstMaxInterval, FarBurstMinInterval, Nearness);
        BurstCooldown = BaseInterval * FMath::FRandRange(0.75f, 1.25f);
    }
}

float UPOSTRadioComponent::ShapeContinuousInterference(float Proximity) const
{
    const float Denominator = FMath::Max(1.0f - ContinuousInterferenceThreshold, 0.01f);
    const float Alpha = FMath::Clamp((Proximity - ContinuousInterferenceThreshold) / Denominator, 0.0f, 1.0f);

    // Ease-in: the continuous layer starts restrained and becomes oppressive only very close.
    return FMath::Lerp(0.28f, 1.0f, Alpha * Alpha);
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
