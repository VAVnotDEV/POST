// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "EntityMonster/EntityMonster.h"

// Sets default values
AEntityMonster::AEntityMonster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEntityMonster::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEntityMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

