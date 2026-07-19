#include "Gameplay/POSTBarrier.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

APOSTBarrier::APOSTBarrier()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(Root);
    BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BaseMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    MovablePart = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MovablePart"));
    MovablePart->SetupAttachment(Root);
    MovablePart->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MovablePart->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void APOSTBarrier::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!MovablePart)
    {
        SetActorTickEnabled(false);
        return;
    }

    const FRotator Target = bIsOpen ? OpenRotation : ClosedRotation;
    const FRotator NewRotation = FMath::RInterpTo(MovablePart->GetRelativeRotation(), Target, DeltaSeconds, MoveSpeed);
    MovablePart->SetRelativeRotation(NewRotation);

    if (NewRotation.Equals(Target, 0.1f))
    {
        MovablePart->SetRelativeRotation(Target);
        SetActorTickEnabled(false);
    }
}

bool APOSTBarrier::CanInteract_Implementation(AActor* Interactor) const
{
    return bEnabled && !bLocked && IsValid(Interactor);
}

FText APOSTBarrier::GetInteractText_Implementation(AActor* Interactor) const
{
    if (!bEnabled) return FText::GetEmpty();
    if (bLocked) return FText::FromString(TEXT("Шлагбаум заблокирован"));
    return FText::FromString(bIsOpen ? TEXT("Закрыть шлагбаум") : TEXT("Открыть шлагбаум"));
}

void APOSTBarrier::Interact_Implementation(AActor* Interactor)
{
    if (!CanInteract_Implementation(Interactor)) return;
    SetOpen(!bIsOpen);
}

void APOSTBarrier::SetOpen(bool bOpen)
{
    if (bLocked || !bEnabled || bIsOpen == bOpen) return;
    bIsOpen = bOpen;
    SetActorTickEnabled(true);
    OnBarrierStateChanged.Broadcast(bIsOpen);
}

void APOSTBarrier::SetLocked(bool bNewLocked)
{
    bLocked = bNewLocked;
}

void APOSTBarrier::SetEnabled(bool bNewEnabled)
{
    bEnabled = bNewEnabled;
}
