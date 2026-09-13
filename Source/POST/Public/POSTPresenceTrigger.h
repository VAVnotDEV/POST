// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTPresenceTrigger.generated.h"


class UBoxComponent;
class APOSTGameDirector;

UCLASS()
class POST_API APOSTPresenceTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOSTPresenceTrigger();

protected:
	// Called when the game starts or when spawned
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnTriggeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	APOSTGameDirector* Director = nullptr;
};
