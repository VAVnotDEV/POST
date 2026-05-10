// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlashLightItem.generated.h"


UCLASS()
class POST_API AFlashLightItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlashLightItem();	void ToggleFlashLight();


protected:
	// Called when the game starts or when spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UStaticMeshComponent* FlashlightStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class USpotLightComponent* LightComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	bool bIsFlashlightOn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	float MaxIntensity = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	float MinIntensity = 0.0f;

	bool isOn() const { return bIsFlashlightOn; }

	virtual void BeginPlay() override;

private:
	void setFlashLightEnabled();
	void setFlashLightDisabled();

};
