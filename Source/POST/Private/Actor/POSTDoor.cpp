#include "Actor/POSTDoor.h"
#include "Components/StaticMeshComponent.h"

APOSTDoor::APOSTDoor()
{
    PrimaryActorTick.bCanEverTick = false;
    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    SetRootComponent(DoorMesh);
}

bool APOSTDoor::CanInteract_Implementation(AActor* Interactor) const { return IsValid(Interactor); }

void APOSTDoor::Interact_Implementation(AActor* Interactor)
{
    if (bIsLocked) return;
    bIsOpen = !bIsOpen;
    SetActorRotation(bIsOpen ? OpenRotation : ClosedRotation);
}

FText APOSTDoor::GetInteractText_Implementation(AActor* Interactor) const
{
    if (bIsLocked) return FText::FromString(TEXT("Заперто"));
    return FText::FromString(bIsOpen ? TEXT("Закрыть дверь") : TEXT("Открыть дверь"));
}
