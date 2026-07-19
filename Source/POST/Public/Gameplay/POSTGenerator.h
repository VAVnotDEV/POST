#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTGenerator.generated.h"

class APOSTFuelContainer;
class APOSTGameState;
class APOSTGameDirector;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTGeneratorStateChanged, EPOSTGeneratorState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTGeneratorFuelChanged, float, NewFuel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTGeneratorConditionChanged, float, NewCondition);

UCLASS(Blueprintable)
class POST_API APOSTGenerator : public AActor, public IInteractable
{
    GENERATED_BODY()
public:
    APOSTGenerator();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(BlueprintAssignable, Category="POST|Generator") FPOSTGeneratorStateChanged OnStateChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Generator") FPOSTGeneratorFuelChanged OnFuelChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Generator") FPOSTGeneratorConditionChanged OnConditionChanged;

    UFUNCTION(BlueprintCallable, Category="POST|Generator") bool StartGenerator();
    UFUNCTION(BlueprintCallable, Category="POST|Generator") void StopGenerator();
    UFUNCTION(BlueprintCallable, Category="POST|Generator") void BreakGenerator();
    UFUNCTION(BlueprintCallable, Category="POST|Generator") void RepairGenerator(float RepairAmount = 0.35f);
    UFUNCTION(BlueprintCallable, Category="POST|Generator") float RefuelFrom(APOSTFuelContainer* Container, float RequestedAmount = 20.0f);
    UFUNCTION(BlueprintCallable, Category="POST|Generator") float AddFuel(float Amount);
    UFUNCTION(BlueprintCallable, Category="POST|Generator") void ApplyReliabilityMultiplier(float Multiplier);

    UFUNCTION(BlueprintPure, Category="POST|Generator") bool HasPower() const { return State == EPOSTGeneratorState::Running && Fuel > 0.0f; }
    UFUNCTION(BlueprintPure, Category="POST|Generator") float GetFuel() const { return Fuel; }
    UFUNCTION(BlueprintPure, Category="POST|Generator") float GetFuelPercent() const { return FuelCapacity > 0.0f ? Fuel / FuelCapacity : 0.0f; }
    UFUNCTION(BlueprintPure, Category="POST|Generator") float GetCondition() const { return Condition; }
    UFUNCTION(BlueprintPure, Category="POST|Generator") EPOSTGeneratorState GetState() const { return State; }

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractText_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator", meta=(ClampMin="1.0")) float FuelCapacity = 40.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator", meta=(ClampMin="0.0")) float Fuel = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator", meta=(ClampMin="0.0")) float FuelConsumptionPerGameMinute = 0.25f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator", meta=(ClampMin="0.0", ClampMax="1.0")) float Condition = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator", meta=(ClampMin="0.0")) float WearPerGameMinute = 0.002f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator", meta=(ClampMin="0.0", ClampMax="1.0")) float RandomFailureChancePerGameMinute = 0.01f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator", meta=(ClampMin="0.1")) float ReliabilityMultiplier = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator") float RepairAmountPerInteraction = 0.35f;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="POST|Generator") APOSTFuelContainer* DefaultFuelSource = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Generator") float RefuelAmountPerInteraction = 20.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Generator") EPOSTGeneratorState State = EPOSTGeneratorState::Stopped;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Generator") void OnGeneratorStarted();
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Generator") void OnGeneratorStopped();
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Generator") void OnGeneratorBroken();

private:
    UPROPERTY() APOSTGameState* CachedGameState = nullptr;
    UFUNCTION() void HandleGameMinuteChanged(int32 NewMinute);
    void SimulateGameMinute();
    void SetState(EPOSTGeneratorState NewState);
};
