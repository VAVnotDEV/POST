#include "Gameplay/POSTSleepPoint.h"
#include "Gameplay/POSTGameDirector.h"
#include "POSTGameState.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

APOSTSleepPoint::APOSTSleepPoint()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void APOSTSleepPoint::BeginPlay()
{
    Super::BeginPlay();
    if (!Director && bAutoFindDirector)
    {
        Director = Cast<APOSTGameDirector>(UGameplayStatics::GetActorOfClass(this, APOSTGameDirector::StaticClass()));
    }
}

bool APOSTSleepPoint::CanInteract_Implementation(AActor* Interactor) const
{
    return bCanSleep && IsValid(Interactor);
}

FText APOSTSleepPoint::GetInteractText_Implementation(AActor* Interactor) const
{
    return bCanSleep ? FText::FromString(TEXT("Лечь спать")) : FText::FromString(TEXT("Сейчас нельзя спать"));
}

void APOSTSleepPoint::Interact_Implementation(AActor* Interactor)
{
    if (!CanInteract_Implementation(Interactor)) return;
    if (APOSTGameState* GameState = GetWorld() ? GetWorld()->GetGameState<APOSTGameState>() : nullptr)
    {
        const int32 NextDay = GameState->GetDay() + 1;
        GameState->SetGameTime(NextDay, WakeHour, 0, 0);
    }

    if (Director) Director->SaveProgress();
    OnSleepRequested(Interactor);
}
