#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTInteractionComponent.generated.h"

class UCameraComponent;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOSTFocusedActorChanged, AActor*, NewActor, FText, InteractionText);

UCLASS(ClassGroup=(POST), meta=(BlueprintSpawnableComponent))
class POST_API UPOSTInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPOSTInteractionComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="POST|Interaction")
    void TryInteract();

    UFUNCTION(BlueprintCallable, Category="POST|Interaction")
    void UpdateInteractActor();

    UFUNCTION(BlueprintPure, Category="POST|Interaction")
    AActor* GetCurrentInteractActor() const { return CurrentInteractActor; }

    UFUNCTION(BlueprintPure, Category="POST|Interaction")
    FText GetCurrentInteractText() const { return CurrentInteractText; }

    UPROPERTY(BlueprintAssignable, Category="POST|Interaction")
    FPOSTFocusedActorChanged OnFocusedActorChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Interaction", meta=(ClampMin="50.0"))
    float InteractDistance = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Interaction")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Interaction")
    bool bDrawDebugTrace = false;

private:
    UPROPERTY()
    AActor* CurrentInteractActor = nullptr;

    UPROPERTY()
    UCameraComponent* Camera = nullptr;

    FText CurrentInteractText;
    void SetFocusedActor(AActor* NewActor, const FText& NewText);
};
