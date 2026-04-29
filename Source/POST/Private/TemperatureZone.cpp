// Fill out your copyright notice in the Description page of Project Settings.


#include "TemperatureZone.h"
#include "Components\BoxComponent.h"
#include "Player\POSTCharacter.h"

// Sets default values
ATemperatureZone::ATemperatureZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SetRootComponent(Box);

}

// Called when the game starts or when spawned
void ATemperatureZone::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &ATemperatureZone::OnBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ATemperatureZone::OnEndOverlap);
}

void ATemperatureZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{


}

void ATemperatureZone::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


