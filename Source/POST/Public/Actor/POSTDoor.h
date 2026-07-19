#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "POSTDoor.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class POST_API APOSTDoor : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    APOSTDoor();
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractText_Implementation(AActor* Interactor) const override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") UStaticMeshComponent* DoorMesh = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door") bool bIsLocked = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Door") bool bIsOpen = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door") FRotator ClosedRotation = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door") FRotator OpenRotation = FRotator(0.0f, 90.0f, 0.0f);
};
