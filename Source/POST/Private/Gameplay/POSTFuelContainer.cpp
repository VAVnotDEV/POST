#include "Gameplay/POSTFuelContainer.h"
#include "Components/SceneComponent.h"

APOSTFuelContainer::APOSTFuelContainer()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    InteractionText = FText::FromString(TEXT("Топливо"));
}
float APOSTFuelContainer::AddFuel(float Amount)
{
    const float Accepted = FMath::Clamp(Amount, 0.0f, Capacity - Fuel);
    Fuel += Accepted;
    return Accepted;
}
float APOSTFuelContainer::RemoveFuel(float Amount)
{
    const float Removed = FMath::Clamp(Amount, 0.0f, Fuel);
    Fuel -= Removed;
    return Removed;
}
void APOSTFuelContainer::ApplyResourceMultiplier(float Multiplier)
{
    Fuel = FMath::Clamp(Fuel * FMath::Max(0.0f, Multiplier), 0.0f, Capacity);
}
bool APOSTFuelContainer::CanInteract_Implementation(AActor* Interactor) const { return IsValid(Interactor); }
FText APOSTFuelContainer::GetInteractText_Implementation(AActor* Interactor) const { return InteractionText; }
void APOSTFuelContainer::Interact_Implementation(AActor* Interactor) { OnContainerInteracted(Interactor); }
