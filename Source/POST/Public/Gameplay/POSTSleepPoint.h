#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "POSTSleepPoint.generated.h"

class APOSTGameDirector;
class USceneComponent;

UCLASS(Blueprintable)
class POST_API APOSTSleepPoint : public AActor, public IInteractable
{
    GENERATED_BODY()
public:
    APOSTSleepPoint();
    virtual void BeginPlay() override;
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractText_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;

    UFUNCTION(BlueprintCallable, Category="POST|Sleep") void SetCanSleep(bool bNewCanSleep) { bCanSleep = bNewCanSleep; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="POST|Sleep") APOSTGameDirector* Director = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Sleep") bool bAutoFindDirector = true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Sleep") bool bCanSleep = true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Sleep") int32 WakeHour = 8;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Sleep") void OnSleepRequested(AActor* Interactor);
};
