#include "Gameplay/POSTAnomaly.h"

#include "Gameplay/POSTGameDirector.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

APOSTAnomaly::APOSTAnomaly()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void APOSTAnomaly::BeginPlay()
{
    Super::BeginPlay();
    ResolveDirector();
}

void APOSTAnomaly::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bActive && Director)
    {
        Director->NotifyAnomalyStopped(this);
    }

    GetWorldTimerManager().ClearTimer(StopTimer);
    Super::EndPlay(EndPlayReason);
}

void APOSTAnomaly::ResolveDirector()
{
    if (!Director)
    {
        Director = Cast<APOSTGameDirector>(UGameplayStatics::GetActorOfClass(this, APOSTGameDirector::StaticClass()));
    }
}

bool APOSTAnomaly::CanActivate() const
{
    if (bActive || (bOneShot && bWasActivated))
    {
        return false;
    }

    if (Director && static_cast<uint8>(Director->GetStoryStage()) < static_cast<uint8>(MinimumStage))
    {
        return false;
    }

    if (Cooldown > 0.0f && LastStoppedWorldTime >= 0.0f && GetWorld())
    {
        const float Elapsed = GetWorld()->GetTimeSeconds() - LastStoppedWorldTime;
        if (Elapsed < Cooldown)
        {
            return false;
        }
    }

    return !Director || Director->CanStartAnomaly(this);
}

bool APOSTAnomaly::ActivateAnomaly()
{
    ResolveDirector();

    if (!CanActivate())
    {
        return false;
    }

    bActive = true;
    bWasActivated = true;

    if (Director)
    {
        Director->NotifyAnomalyStarted(this);
    }

    OnAnomalyStarted();

    if (AutoStopDelay > 0.0f)
    {
        GetWorldTimerManager().SetTimer(StopTimer, this, &APOSTAnomaly::StopAnomaly, AutoStopDelay, false);
    }

    return true;
}

void APOSTAnomaly::StopAnomaly()
{
    if (!bActive)
    {
        return;
    }

    bActive = false;
    GetWorldTimerManager().ClearTimer(StopTimer);
    LastStoppedWorldTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

    OnAnomalyStopped();

    if (Director)
    {
        Director->NotifyAnomalyStopped(this);
    }
}
