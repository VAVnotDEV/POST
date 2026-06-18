// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "EntityMonster/POSTEntityPresence.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
APOSTEntityPresence::APOSTEntityPresence()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APOSTEntityPresence::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

// Called every frame
void APOSTEntityPresence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerPawn) return;

	const float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

	ThreatLevel = 1.0f - FMath::Clamp(Distance / MaxDistance, 0.0f, 1.0f);

}

