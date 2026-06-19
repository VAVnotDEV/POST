// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "Components/POSTInteractionComponent.h"
#include "DrawDebugHelpers.h"
#include "Interfaces/Interactable.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UPOSTInteractionComponent::UPOSTInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPOSTInteractionComponent::UpdateInteractActor()
{
    CurrentInteractActor = nullptr;

    if (!GetWorld() || !Camera) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    const FVector Start = Camera->GetComponentLocation();
    const FVector End = Start + Camera->GetForwardVector() * InteractDistance;

    FHitResult HitResult;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        Params
    );

#if WITH_EDITOR
    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        bHit ? FColor::Green : FColor::Red,
        false,
        0.0f,
        0,
        1.0f
    );
#endif

    if (!bHit) return;

    AActor* HitActor = HitResult.GetActor();
    if (!HitActor) return;

    if (!HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
    {
        return;
    }

    if (!IInteractable::Execute_CanInteract(HitActor, Owner))
    {
        return;
    }

    CurrentInteractActor = HitActor;

    const FText InteractText = IInteractable::Execute_GetInteractText(HitActor, Owner);
    UE_LOG(LogTemp, Warning, TEXT("Interact: %s"), *InteractText.ToString());
}

void UPOSTInteractionComponent::TryInteract()
{
    if (!CurrentInteractActor) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    if (!CurrentInteractActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
    {
        return;
    }

    if (!IInteractable::Execute_CanInteract(CurrentInteractActor, Owner))
    {
        return;
    }

    IInteractable::Execute_Interact(CurrentInteractActor, Owner);
}


// Called when the game starts
void UPOSTInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPOSTInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

