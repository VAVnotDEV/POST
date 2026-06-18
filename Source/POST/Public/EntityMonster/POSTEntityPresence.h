// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTEntityPresence.generated.h"

UCLASS()
class POST_API APOSTEntityPresence : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOSTEntityPresence();

	UFUNCTION(BlueprintCallable)
	float GetThreatLevel() const { return ThreatLevel; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	float MaxDistance = 3000.0f;

	UPROPERTY(EditAnywhere)
	float ThreatLevel = 0.0f;

	UPROPERTY()
	APawn* PlayerPawn;
};
