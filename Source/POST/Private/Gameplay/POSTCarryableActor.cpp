#include "Gameplay/POSTCarryableActor.h"

#include "Components/StaticMeshComponent.h"
#include "Player/POSTCharacter.h"

APOSTCarryableActor::APOSTCarryableActor()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    Mesh->SetSimulatePhysics(true);
    Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    TakeText = FText::FromString(TEXT("Взять"));
}

bool APOSTCarryableActor::CanInteract_Implementation(AActor* Interactor) const
{
    const APOSTCharacter* Character = Cast<APOSTCharacter>(Interactor);
    return Character && !bIsCarried && Character->GetCarriedActor() == nullptr;
}

FText APOSTCarryableActor::GetInteractText_Implementation(AActor* Interactor) const
{
    return TakeText;
}

void APOSTCarryableActor::Interact_Implementation(AActor* Interactor)
{
    if (APOSTCharacter* Character = Cast<APOSTCharacter>(Interactor))
    {
        Character->TryCarry(this);
    }
}

void APOSTCarryableActor::AttachToCharacter(APOSTCharacter* Character)
{
    if (!Character || !Character->GetCarryPoint())
    {
        return;
    }

    Carrier = Character;
    bIsCarried = true;
    Mesh->SetSimulatePhysics(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AttachToComponent(Character->GetCarryPoint(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    OnPickedUp(Character);
}

void APOSTCarryableActor::Drop()
{
    if (!bIsCarried)
    {
        return;
    }

    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetSimulatePhysics(true);
    Carrier = nullptr;
    bIsCarried = false;
    OnDropped();
}
