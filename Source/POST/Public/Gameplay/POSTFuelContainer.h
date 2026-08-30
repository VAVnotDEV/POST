#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "POSTFuelContainer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTFuelContainerChanged, float, NewFuel);

class USceneComponent;

UCLASS(Blueprintable)
class POST_API APOSTFuelContainer : public AActor, public IInteractable
{
    GENERATED_BODY()
public:
    APOSTFuelContainer();
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintAssignable, Category="POST|Fuel") FPOSTFuelContainerChanged OnFuelChanged;

    UFUNCTION(BlueprintPure, Category="POST|Fuel") float GetFuel() const { return Fuel; }
    UFUNCTION(BlueprintPure, Category="POST|Fuel") float GetCapacity() const { return Capacity; }
    UFUNCTION(BlueprintCallable, Category="POST|Fuel") float AddFuel(float Amount);
    UFUNCTION(BlueprintCallable, Category="POST|Fuel") float RemoveFuel(float Amount);

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractText_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Fuel", meta=(ClampMin="0.0")) float Capacity = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Fuel", meta=(ClampMin="0.0")) float Fuel = 10.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Fuel") FText InteractionText;
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Fuel") void OnContainerInteracted(AActor* Interactor);
};
