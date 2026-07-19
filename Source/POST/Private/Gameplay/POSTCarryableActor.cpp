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

bool APOSTCarryableActor::AttachToCharacter(APOSTCharacter* Character)
{
    if (!IsValid(Character) || !Character->GetCarryPoint() || bIsCarried || !Mesh)
    {
        return false;
    }

    Mesh->SetSimulatePhysics(false);
    Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AttachToComponent(
        Character->GetCarryPoint(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale);

    const bool bAttached = GetRootComponent() &&
        GetRootComponent()->GetAttachParent() == Character->GetCarryPoint();

    if (!bAttached)
    {
        Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Mesh->SetSimulatePhysics(true);
        return false;
    }

    Carrier = Character;
    bIsCarried = true;
    OnPickedUp(Character);
    return true;
}

void APOSTCarryableActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (IsValid(Carrier))
    {
        Carrier->NotifyCarriedActorReleased(this);
    }

    Carrier = nullptr;
    bIsCarried = false;
    Super::EndPlay(EndPlayReason);
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
    APOSTCharacter* PreviousCarrier = Carrier;
    Carrier = nullptr;
    bIsCarried = false;

    if (IsValid(PreviousCarrier))
    {
        PreviousCarrier->NotifyCarriedActorReleased(this);
    }

    OnDropped();
}
