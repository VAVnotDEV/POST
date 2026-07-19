#include "EntityMonster/POSTAnomalyDirector.h"

APOSTAnomalyDirector::APOSTAnomalyDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APOSTAnomalyDirector::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("POSTAnomalyDirector is deprecated. Use POSTGameDirector."));
}

void APOSTAnomalyDirector::TryActivateAnomaly()
{
}
