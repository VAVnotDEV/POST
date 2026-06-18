// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTAnomalyDirector.generated.h"

class APOSTEntityPresence;

UCLASS()
class POST_API APOSTAnomalyDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOSTAnomalyDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void TryActivateAnomaly();

	FTimerHandle AnomalyTimerHandle;

	UPROPERTY()
	APOSTEntityPresence* EntityPresence;

};
