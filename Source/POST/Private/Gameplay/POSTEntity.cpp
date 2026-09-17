#include "Gameplay/POSTEntity.h"

#include "AIController.h"
#include "Gameplay/POSTCycleManager.h"
#include "Gameplay/POSTProtectionZone.h"
#include "Kismet/GameplayStatics.h"
#include "Player/POSTCharacter.h"
#include "POSTGameState.h"

APOSTEntity::APOSTEntity()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APOSTEntity::BeginPlay()
{
    Super::BeginPlay();
    Player = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void APOSTEntity::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsValid(Player))
    {
        Player = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (!Player)
        {
            return;
        }
    }

    UpdatePerception(DeltaTime);
    UpdateDecision();
}

void APOSTEntity::UpdatePerception(float DeltaTime)
{
    if (APOSTProtectionZone::IsActorProtected(this, Player))
    {
        Awareness = FMath::Max(0.0f, Awareness - AwarenessDecayPerSecond * DeltaTime);
        return;
    }

    const float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    const float Speed = Player->GetVelocity().Size2D();
    bool bReceivedStimulus = false;

    if (Distance <= PerceptionRadius && Speed > 5.0f)
    {
        const float SpeedAlpha = FMath::Clamp(Speed / 600.0f, 0.15f, 1.0f);
        const float RunModifier = Player->IsRunning() ? RunningMultiplier : 1.0f;
        const float Gain = MovementAwarenessPerSecond * SpeedAlpha * RunModifier *
            GetDistanceMultiplier(Distance) * GetTimeOfDayMultiplier() * DeltaTime;

        Awareness = FMath::Clamp(Awareness + Gain, 0.0f, 100.0f);
        LastKnownPlayerLocation = Player->GetActorLocation();
        bHasLastKnownLocation = true;
        bReceivedStimulus = true;
    }

    if (!bReceivedStimulus)
    {
        Awareness = FMath::Max(0.0f, Awareness - AwarenessDecayPerSecond * DeltaTime);
    }
}

void APOSTEntity::ReportNoise(const FVector& WorldLocation, float Strength)
{
    const float Distance = FVector::Dist(GetActorLocation(), WorldLocation);
    if (Distance > PerceptionRadius)
    {
        return;
    }

    Awareness = FMath::Clamp(Awareness + 30.0f * FMath::Max(0.0f, Strength) *
        GetDistanceMultiplier(Distance) * GetTimeOfDayMultiplier(), 0.0f, 100.0f);
    LastKnownPlayerLocation = WorldLocation;
    bHasLastKnownLocation = true;
}

void APOSTEntity::ReportLight(const FVector& WorldLocation, float Strength)
{
    const float Distance = FVector::Dist(GetActorLocation(), WorldLocation);
    if (Distance > PerceptionRadius)
    {
        return;
    }

    Awareness = FMath::Clamp(Awareness + 18.0f * FMath::Max(0.0f, Strength) *
        GetDistanceMultiplier(Distance) * GetTimeOfDayMultiplier(), 0.0f, 100.0f);
    LastKnownPlayerLocation = WorldLocation;
    bHasLastKnownLocation = true;
}

void APOSTEntity::UpdateDecision()
{
    const bool bPlayerProtected = APOSTProtectionZone::IsActorProtected(this, Player);

    if (bPlayerProtected)
    {
        bAttackCommitted = false;
        if (bHasLastKnownLocation && Awareness >= InterestedThreshold)
        {
            SetEntityState(EPOSTEntityState::Searching);
            MoveToward(LastKnownPlayerLocation);
        }
        else
        {
            SetEntityState(EPOSTEntityState::Roaming);
            StopEntityMovement();
        }
        return;
    }

    if (Awareness >= HuntingThreshold)
    {
        SetEntityState(EPOSTEntityState::Hunting);
        LastKnownPlayerLocation = Player->GetActorLocation();
        bHasLastKnownLocation = true;
        MoveToward(LastKnownPlayerLocation);
        TryAttack();
        return;
    }

    bAttackCommitted = false;

    if (Awareness >= InterestedThreshold && bHasLastKnownLocation)
    {
        SetEntityState(EPOSTEntityState::Searching);
        MoveToward(LastKnownPlayerLocation);
        return;
    }

    SetEntityState(EPOSTEntityState::Roaming);
}

void APOSTEntity::TryAttack()
{
    if (bAttackCommitted || !Player || APOSTProtectionZone::IsActorProtected(this, Player))
    {
        return;
    }

    if (FVector::DistSquared(GetActorLocation(), Player->GetActorLocation()) > FMath::Square(AttackDistance))
    {
        return;
    }

    bAttackCommitted = true;
    SetEntityState(EPOSTEntityState::Attacking);
    StopEntityMovement();
    OnAttackPlayer(Player);

    if (APOSTCycleManager* CycleManager = Cast<APOSTCycleManager>(UGameplayStatics::GetActorOfClass(this, APOSTCycleManager::StaticClass())))
    {
        CycleManager->EndCycle(EPOSTDeathCause::Entity);
    }
}

void APOSTEntity::MoveToward(const FVector& Location)
{
    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        AI->MoveToLocation(Location, SearchAcceptanceRadius, true, true, false, true);
    }
}

void APOSTEntity::StopEntityMovement()
{
    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        AI->StopMovement();
    }
}

void APOSTEntity::SetEntityState(EPOSTEntityState NewState)
{
    if (EntityState == NewState)
    {
        return;
    }

    const EPOSTEntityState OldState = EntityState;
    EntityState = NewState;
    OnEntityStateChanged.Broadcast(OldState, EntityState);
}

float APOSTEntity::GetTimeOfDayMultiplier() const
{
    const APOSTGameState* GameState = GetWorld() ? GetWorld()->GetGameState<APOSTGameState>() : nullptr;
    return GameState && GameState->IsNight() ? NightAwarenessMultiplier : 1.0f;
}

float APOSTEntity::GetDistanceMultiplier(float Distance) const
{
    if (PerceptionRadius <= KINDA_SMALL_NUMBER)
    {
        return 0.0f;
    }

    return FMath::Clamp(1.0f - Distance / PerceptionRadius, 0.1f, 1.0f);
}
