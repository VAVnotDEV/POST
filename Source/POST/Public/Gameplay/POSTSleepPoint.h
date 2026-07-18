#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "POSTSleepPoint.generated.h"
class APOSTGameDirector;
UCLASS(Blueprintable)
class POST_API APOSTSleepPoint : public AActor, public IInteractable
{
    GENERATED_BODY()
public:
    APOSTSleepPoint();
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractText_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="POST|Sleep") APOSTGameDirector* Director;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Sleep") bool bCanSleep = true;
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Sleep") void OnSleepRequested(AActor* Interactor);
};
