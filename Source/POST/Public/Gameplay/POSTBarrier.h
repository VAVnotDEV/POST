#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "POSTBarrier.generated.h"

class USceneComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTBarrierStateChanged, bool, bIsOpen);

UCLASS(Blueprintable)
class POST_API APOSTBarrier : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    APOSTBarrier();
    virtual void Tick(float DeltaSeconds) override;

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractText_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;

    UFUNCTION(BlueprintCallable, Category="POST|Barrier") void SetOpen(bool bOpen);
    UFUNCTION(BlueprintCallable, Category="POST|Barrier") void SetLocked(bool bNewLocked);
    UFUNCTION(BlueprintCallable, Category="POST|Barrier") void SetEnabled(bool bNewEnabled);
    UFUNCTION(BlueprintPure, Category="POST|Barrier") bool IsOpen() const { return bIsOpen; }
    UFUNCTION(BlueprintPure, Category="POST|Barrier") bool IsLocked() const { return bLocked; }

    UPROPERTY(BlueprintAssignable, Category="POST|Barrier") FPOSTBarrierStateChanged OnBarrierStateChanged;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Barrier|Components")
    USceneComponent* Root = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Barrier|Components")
    UStaticMeshComponent* BaseMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Barrier|Components")
    UStaticMeshComponent* MovablePart = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Barrier") FRotator ClosedRotation = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Barrier") FRotator OpenRotation = FRotator(0, 0, 80);
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Barrier", meta=(ClampMin="0.01")) float MoveSpeed = 2.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Barrier") bool bIsOpen = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Barrier") bool bLocked = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Barrier") bool bEnabled = true;
};
