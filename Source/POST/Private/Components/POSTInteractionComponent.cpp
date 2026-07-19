#include "Components/POSTInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Interfaces/Interactable.h"

UPOSTInteractionComponent::UPOSTInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.05f;
}

void UPOSTInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
    Camera = GetOwner() ? GetOwner()->FindComponentByClass<UCameraComponent>() : nullptr;
    if (!Camera)
    {
        UE_LOG(LogTemp, Error, TEXT("POSTInteractionComponent: camera not found on %s"), *GetNameSafe(GetOwner()));
        SetComponentTickEnabled(false);
    }
}

void UPOSTInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateInteractActor();
}

void UPOSTInteractionComponent::UpdateInteractActor()
{
    if (!GetWorld() || !Camera || !GetOwner())
    {
        SetFocusedActor(nullptr, FText::GetEmpty());
        return;
    }

    const FVector Start = Camera->GetComponentLocation();
    const FVector End = Start + Camera->GetForwardVector() * InteractDistance;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(POSTInteraction), false, GetOwner());
    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);

    if (bDrawDebugTrace)
    {
        DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.06f, 0, 1.0f);
    }

    AActor* HitActor = bHit ? Hit.GetActor() : nullptr;
    if (!HitActor || !HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()) ||
        !IInteractable::Execute_CanInteract(HitActor, GetOwner()))
    {
        SetFocusedActor(nullptr, FText::GetEmpty());
        return;
    }

    SetFocusedActor(HitActor, IInteractable::Execute_GetInteractText(HitActor, GetOwner()));
}

void UPOSTInteractionComponent::TryInteract()
{
    AActor* Target = CurrentInteractActor;
    if (!IsValid(Target) || !GetOwner()) return;
    if (!Target->GetClass()->ImplementsInterface(UInteractable::StaticClass())) return;
    if (!IInteractable::Execute_CanInteract(Target, GetOwner())) return;

    IInteractable::Execute_Interact(Target, GetOwner());
    UpdateInteractActor();
}

void UPOSTInteractionComponent::SetFocusedActor(AActor* NewActor, const FText& NewText)
{
    const bool bChanged = CurrentInteractActor != NewActor || !CurrentInteractText.EqualTo(NewText);
    CurrentInteractActor = NewActor;
    CurrentInteractText = NewText;
    if (bChanged)
    {
        OnFocusedActorChanged.Broadcast(CurrentInteractActor, CurrentInteractText);
    }
}
