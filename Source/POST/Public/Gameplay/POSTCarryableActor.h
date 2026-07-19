#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "POSTCarryableActor.generated.h"

class UStaticMeshComponent;
class APOSTCharacter;

UCLASS(Blueprintable)
class POST_API APOSTCarryableActor : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    APOSTCarryableActor();
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractText_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;

    UFUNCTION(BlueprintCallable, Category="POST|Carry")
    bool AttachToCharacter(APOSTCharacter* Character);

    UFUNCTION(BlueprintCallable, Category="POST|Carry")
    void Drop();

    UFUNCTION(BlueprintPure, Category="POST|Carry")
    bool IsCarried() const { return bIsCarried; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Carry")
    UStaticMeshComponent* Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Carry")
    FText TakeText;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Carry")
    void OnPickedUp(APOSTCharacter* Character);

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Carry")
    void OnDropped();

private:
    UPROPERTY(Transient)
    APOSTCharacter* Carrier = nullptr;

    bool bIsCarried = false;
};
