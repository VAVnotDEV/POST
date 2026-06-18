// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "Actor/POSTDoor.h"

// Sets default values
APOSTDoor::APOSTDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
//void APOSTDoor::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void APOSTDoor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

bool APOSTDoor::CanInteract_Implementation(AActor* Interactor) const
{
    return true;
}

void APOSTDoor::Interact_Implementation(AActor* Interactor)
{
    if (bIsLocked)
    {
        UE_LOG(LogTemp, Warning, TEXT("Door is locked"));
        return;
    }

    bIsOpen = !bIsOpen;

    SetActorRotation(bIsOpen ? OpenRotation : ClosedRotation);

    UE_LOG(LogTemp, Warning, TEXT("Door %s"), bIsOpen ? TEXT("opened") : TEXT("closed"));
}

FText APOSTDoor::GetInteractText_Implementation(AActor* Interactor) const
{
    if (bIsLocked)
    {
        return FText::FromString(TEXT("Locked"));
    }

    return bIsOpen
        ? FText::FromString(TEXT("Close door"))
        : FText::FromString(TEXT("Open door"));
}

