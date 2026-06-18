// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "POSTDoor.generated.h"


UCLASS()
class POST_API APOSTDoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOSTDoor();

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractText_Implementation(AActor* Interactor) const override;

//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, Category = "Door")
	bool bIsLocked = false;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	bool bIsOpen = false;

	UPROPERTY(EditAnywhere, Category = "Door")
	FRotator ClosedRotation = FRotator(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Door")
	FRotator OpenRotation = FRotator(0.0f, 90.0f, 0.0f);

};
