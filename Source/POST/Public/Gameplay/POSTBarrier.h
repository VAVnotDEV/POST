#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "POSTBarrier.generated.h"
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
    UFUNCTION(BlueprintPure, Category="POST|Barrier") bool IsOpen() const { return bIsOpen; }
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* MovablePart;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Barrier") FRotator ClosedRotation = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Barrier") FRotator OpenRotation = FRotator(0,0,80);
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Barrier") float MoveSpeed = 2.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Barrier") bool bIsOpen = false;
};
