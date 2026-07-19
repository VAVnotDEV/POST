// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "POSTCharacter.generated.h"

class AFlashLightItem;
class APOSTCarryableActor;
class UCameraComponent;
class USceneComponent;
class UTextRenderComponent;
class UPOSTTemperatureComponent;
class UPOSTStaminaComponent;
class UPOSTInteractionComponent;
class UPOSTFootstepComponent;
class UPOSTRadioComponent;

UCLASS()
class POST_API APOSTCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APOSTCharacter(const FObjectInitializer& ObjInit);

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintPure, Category="POST|Movement")
    bool IsRunning() const;

    UFUNCTION(BlueprintPure, Category="POST|Components")
    UPOSTTemperatureComponent* GetTemperatureComponent() const { return TemperatureComponent; }

    UFUNCTION(BlueprintPure, Category="POST|Components")
    UPOSTStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }

    UFUNCTION(BlueprintPure, Category="POST|Components")
    UPOSTRadioComponent* GetRadioComponent() const { return RadioComponent; }

    UFUNCTION(BlueprintPure, Category="POST|Carry")
    USceneComponent* GetCarryPoint() const { return CarryPoint; }

    UFUNCTION(BlueprintPure, Category="POST|Carry")
    APOSTCarryableActor* GetCarriedActor() const { return CarriedActor; }

    UFUNCTION(BlueprintCallable, Category="POST|Carry")
    bool TryCarry(APOSTCarryableActor* Actor);

    UFUNCTION(BlueprintCallable, Category="POST|Carry")
    void DropCarriedActor();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UCameraComponent* Camera = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USceneComponent* FlashlightAttachPoint = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USceneComponent* CarryPoint = nullptr;

    UPROPERTY(EditDefaultsOnly, Category="FlashLight")
    TSubclassOf<AFlashLightItem> FlashlightClass;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="FlashLight")
    AFlashLightItem* FlashlightActor = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UTextRenderComponent* StaminaTextComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UTextRenderComponent* TemperatureTextComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UPOSTInteractionComponent* InteractionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UPOSTFootstepComponent* FootstepComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UPOSTRadioComponent* RadioComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UPOSTTemperatureComponent* TemperatureComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UPOSTStaminaComponent* StaminaComponent = nullptr;

private:
    void MoveForward(float Amount);
    void MoveRight(float Amount);
    void SpawnFlashlight();
    void ToggleFlashlight();
    void OnStartRunning();
    void OnStopRunning();
    void TryInteract();

    UFUNCTION()
    void OnBodyTemperatureChanged(float NewTemp);

    UFUNCTION()
    void OnStaminaChanged(float NewStamina);

    UFUNCTION()
    void HandleFrozen();

    bool bIsMovingForward = false;
    bool bWantsToRun = false;

    UPROPERTY(Transient)
    APOSTCarryableActor* CarriedActor = nullptr;
};
