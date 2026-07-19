#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTStaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTStaminaChanged, float, NewStamina);

UCLASS(ClassGroup=(POST), meta=(BlueprintSpawnableComponent))
class POST_API UPOSTStaminaComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPOSTStaminaComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintPure, Category="POST|Stamina") bool HasStamina() const { return CurrentStamina > KINDA_SMALL_NUMBER; }
    UFUNCTION(BlueprintPure, Category="POST|Stamina") bool CanRun() const { return CurrentStamina >= MinStaminaToRun; }
    UFUNCTION(BlueprintPure, Category="POST|Stamina") float GetCurrentStamina() const { return CurrentStamina; }
    UFUNCTION(BlueprintPure, Category="POST|Stamina") float GetStaminaPercent() const { return MaxStamina > 0.0f ? CurrentStamina / MaxStamina : 0.0f; }
    UFUNCTION(BlueprintCallable, Category="POST|Stamina") void StartSpendStamina();
    UFUNCTION(BlueprintCallable, Category="POST|Stamina") void StopSpendStamina();
    UFUNCTION(BlueprintCallable, Category="POST|Stamina") void RestoreStamina(float Amount);

    UPROPERTY(BlueprintAssignable, Category="POST|Stamina") FPOSTStaminaChanged OnStaminaChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Stamina", meta=(ClampMin="1.0")) float MaxStamina = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Stamina", meta=(ClampMin="0.0")) float StaminaSpendPerSecond = 10.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Stamina", meta=(ClampMin="0.0")) float StaminaRegenPerSecond = 5.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Stamina", meta=(ClampMin="0.0")) float MinStaminaToRun = 5.0f;

private:
    UPROPERTY(VisibleAnywhere, Category="POST|Stamina") float CurrentStamina = 0.0f;
    bool bIsSpendingStamina = false;
    void SetStamina(float NewStamina);
};
