#include "Gameplay/POSTPowerConsumer.h"
#include "Gameplay/POSTGenerator.h"
#include "Components/SceneComponent.h"
APOSTPowerConsumer::APOSTPowerConsumer(){ PrimaryActorTick.bCanEverTick=true; PrimaryActorTick.TickInterval=0.25f; Root=CreateDefaultSubobject<USceneComponent>(TEXT("Root")); SetRootComponent(Root); }
void APOSTPowerConsumer::BeginPlay(){ Super::BeginPlay(); RefreshPower(); }
void APOSTPowerConsumer::Tick(float DeltaSeconds){ Super::Tick(DeltaSeconds); RefreshPower(); }
void APOSTPowerConsumer::SetEnabled(bool bNewEnabled){ bEnabled=bNewEnabled; RefreshPower(); }
void APOSTPowerConsumer::RefreshPower(){ const bool NewPowered=bEnabled && IsValid(Generator) && Generator->HasPower(); if(NewPowered!=bPowered){ bPowered=NewPowered; OnPowerChanged(bPowered);} }
