// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "POSTCharacter.generated.h"



class AFlashLightItem;
class UPOSTTemperatureComponent;
class UPOSTStaminaComponent;
class UTextRenderComponent;
class UPOSTEntityAudioComponent;

UCLASS()
class POST_API APOSTCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APOSTCharacter(const FObjectInitializer& ObjInit);

protected:
	// Called when the game starts or when spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* FlashlightAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "FlashLight")
	TSubclassOf<AFlashLightItem> FlashlightClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FlashLight")
	 AFlashLightItem* FlashlightActor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTextRenderComponent* StaminaTextComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTextRenderComponent* TemperaturaTextComponent;

	UPROPERTY(VisibleAnywhere)
	UPOSTEntityAudioComponent* EntityAudioComponent;

	 UFUNCTION(BlueprintCallable)
	 void ToggleFlashlight();

	virtual void BeginPlay() override;


	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsRunning() const;

	UPOSTTemperatureComponent* GetTemperatureComponent() const;
	UPOSTStaminaComponent* GetStaminaComponent() const;

private:
	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void SpawnFlashlight();
	bool IsMovingForward = false;
	bool WantsToRun = false;

	bool bIsFrozen = false;

	void OnStartRunning();
	void OnStopRunning();

	UPROPERTY(EditAnywhere, Category = "Component")
	UPOSTTemperatureComponent* TemperatureComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UPOSTStaminaComponent* StaminaComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	float InteractDistance = 300.0f;

	UPROPERTY()
	AActor* CurrentInteractActor = nullptr;

	void UpdateInteractActor();

	void TryInteract();
	
};
