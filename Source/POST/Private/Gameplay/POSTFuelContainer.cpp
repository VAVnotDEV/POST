#include "Gameplay/POSTFuelContainer.h"

#include "Components/SceneComponent.h"
#include "Gameplay/POSTGameDirector.h"
#include "Kismet/GameplayStatics.h"

APOSTFuelContainer::APOSTFuelContainer()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    InteractionText = FText::FromString(TEXT("Топливо"));
}

void APOSTFuelContainer::BeginPlay()
{
    Super::BeginPlay();

    Capacity = FMath::Max(0.0f, Capacity);
    Fuel = FMath::Clamp(Fuel, 0.0f, Capacity);

    if (const APOSTGameDirector* Director = Cast<APOSTGameDirector>(
        UGameplayStatics::GetActorOfClass(this, APOSTGameDirector::StaticClass())))
    {
        ApplyResourceMultiplier(Director->GetWorldResourceMultiplier());
    }
    else
    {
        OnFuelChanged.Broadcast(Fuel);
    }
}

float APOSTFuelContainer::AddFuel(float Amount)
{
    const float Accepted = FMath::Clamp(Amount, 0.0f, Capacity - Fuel);
    if (Accepted <= KINDA_SMALL_NUMBER)
    {
        return 0.0f;
    }

    Fuel += Accepted;
    OnFuelChanged.Broadcast(Fuel);
    return Accepted;
}

float APOSTFuelContainer::RemoveFuel(float Amount)
{
    const float Removed = FMath::Clamp(Amount, 0.0f, Fuel);
    if (Removed <= KINDA_SMALL_NUMBER)
    {
        return 0.0f;
    }

    Fuel -= Removed;
    OnFuelChanged.Broadcast(Fuel);
    return Removed;
}

void APOSTFuelContainer::ApplyResourceMultiplier(float Multiplier)
{
    Fuel = FMath::Clamp(Fuel * FMath::Max(0.0f, Multiplier), 0.0f, Capacity);
    OnFuelChanged.Broadcast(Fuel);
}

bool APOSTFuelContainer::CanInteract_Implementation(AActor* Interactor) const
{
    return IsValid(Interactor);
}

FText APOSTFuelContainer::GetInteractText_Implementation(AActor* Interactor) const
{
    return InteractionText;
}

void APOSTFuelContainer::Interact_Implementation(AActor* Interactor)
{
    OnContainerInteracted(Interactor);
}
