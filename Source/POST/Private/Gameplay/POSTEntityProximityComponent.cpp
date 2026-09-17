#include "Gameplay/POSTEntityProximityComponent.h"

#include "Gameplay/POSTEntity.h"
#include "Kismet/GameplayStatics.h"

UPOSTEntityProximityComponent::UPOSTEntityProximityComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPOSTEntityProximityComponent::BeginPlay()
{
    Super::BeginPlay();
    Entity = Cast<APOSTEntity>(UGameplayStatics::GetActorOfClass(this, APOSTEntity::StaticClass()));
}

void UPOSTEntityProximityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    if (!IsValid(Entity))
    {
        Entity = Cast<APOSTEntity>(UGameplayStatics::GetActorOfClass(this, APOSTEntity::StaticClass()));
    }

    float TargetIntensity = 0.0f;

    if (Entity)
    {
        EntityDistance = FVector::Dist(Owner->GetActorLocation(), Entity->GetActorLocation());

        if (MaxSignalDistance > FullSignalDistance)
        {
            TargetIntensity = 1.0f - FMath::GetRangePct(FullSignalDistance, MaxSignalDistance, EntityDistance);
            TargetIntensity = FMath::Clamp(TargetIntensity, 0.0f, 1.0f);
        }
        else
        {
            TargetIntensity = EntityDistance <= FullSignalDistance ? 1.0f : 0.0f;
        }
    }
    else
    {
        EntityDistance = 0.0f;
    }

    const float PreviousIntensity = ProximityIntensity;
    ProximityIntensity = FMath::FInterpTo(ProximityIntensity, TargetIntensity, DeltaTime, InterpSpeed);

    if (!FMath::IsNearlyEqual(PreviousIntensity, ProximityIntensity, 0.001f))
    {
        OnProximityChanged.Broadcast(ProximityIntensity);
    }
}
