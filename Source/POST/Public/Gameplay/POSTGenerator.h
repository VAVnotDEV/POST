#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTGenerator.generated.h"

class APOSTFuelContainer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTGeneratorStateChanged, EPOSTGeneratorState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTGeneratorFuelChanged, float, NewFuel);

UCLASS(Blueprintable)
class POST_API APOSTGenerator : public AActor, public IInteractable
{
    GENERATED_BODY()
public:
    APOSTGenerator();
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(BlueprintAssignable, Category="POST|Generator") FPOSTGeneratorStateChanged OnStateChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Generator") FPOSTGeneratorFuelChanged OnFuelChanged;

    UFUNCTION(BlueprintCallable, Category="POST|Generator") bool StartGenerator();
    UFUNCTION(BlueprintCallable, Category="POST|Generator") void StopGenerator();
    UFUNCTION(BlueprintCallable, Category="POST|Generator") void BreakGenerator();
    UFUNCTION(BlueprintCallable, Category="POST|Generator") void RepairGenerator(float RepairAmount = 1.0f);
    UFUNCTION(BlueprintCallable, Category="POST|Generator") float RefuelFrom(APOSTFuelContainer* Container, float RequestedAmount);
    UFUNCTION(BlueprintPure, Category="POST|Generator") bool HasPower() const { return State == EPOSTGeneratorState::Running && Fuel > 0.0f; }
    UFUNCTION(BlueprintPure, Category="POST|Generator") float GetFuel() const { return Fuel; }
    UFUNCTION(BlueprintPure, Category="POST|Generator") float GetCondition() const { return Condition; }
    UFUNCTION(BlueprintPure, Category="POST|Generator") EPOSTGeneratorState GetState() const { return State; }

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractText_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator") float FuelCapacity = 40.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator") float Fuel = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator") float FuelConsumptionPerMinute = 0.25f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator") float Condition = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator") float WearPerMinute = 0.002f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator") float RandomFailureChancePerMinute = 0.01f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator") float ReliabilityMultiplier = 1.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Generator") EPOSTGeneratorState State = EPOSTGeneratorState::Stopped;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Generator") void OnGeneratorStarted();
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Generator") void OnGeneratorStopped();
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Generator") void OnGeneratorBroken();

private:
    float RuntimeAccumulator = 0.0f;
    void SetState(EPOSTGeneratorState NewState);
    void SimulateMinute();
};
