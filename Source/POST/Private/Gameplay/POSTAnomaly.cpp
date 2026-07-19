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

bool APOSTAnomaly::ActivateAnomaly()
{
    if (!Director)
    {
        Director = Cast<APOSTGameDirector>(UGameplayStatics::GetActorOfClass(this, APOSTGameDirector::StaticClass()));
    }
    if (bActive || (bOneShot && bWasActivated)) return false;
    if (Director && static_cast<uint8>(Director->GetStoryStage()) < static_cast<uint8>(MinimumStage)) return false;

    bActive = true;
    bWasActivated = true;
    OnAnomalyStarted();
    if (AutoStopDelay > 0.0f)
    {
        GetWorldTimerManager().SetTimer(StopTimer, this, &APOSTAnomaly::StopAnomaly, AutoStopDelay, false);
    }
    return true;
}

void APOSTAnomaly::StopAnomaly()
{
    if (!bActive) return;
    bActive = false;
    GetWorldTimerManager().ClearTimer(StopTimer);
    OnAnomalyStopped();
}
