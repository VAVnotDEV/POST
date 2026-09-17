#include "Gameplay/POSTEntity.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/POSTCycleManager.h"
#include "Gameplay/POSTProtectionZone.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
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
    SpawnLocation = GetActorLocation();
    TargetPlayer = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    RoamingMoveCooldown = 0.0f;
    LocationSampleCooldown = 0.0f;
    ApplyMovementSpeedForState(EntityState);
}

void APOSTEntity::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsValid(TargetPlayer))
    {
        TargetPlayer = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (!TargetPlayer)
        {
            return;
        }
    }

    LocationSampleCooldown = FMath::Max(0.0f, LocationSampleCooldown - DeltaTime);
    bReceivedPlayerStimulusThisFrame = false;
    UpdatePerception(DeltaTime);
    UpdateDecision(DeltaTime);
}

void APOSTEntity::UpdatePerception(float DeltaTime)
{
    if (APOSTProtectionZone::IsActorProtected(this, TargetPlayer))
    {
        Awareness = FMath::Max(0.0f, Awareness - AwarenessDecayPerSecond * DeltaTime);
        return;
    }

    const float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
    const float Speed = TargetPlayer->GetVelocity().Size2D();

    if (Distance <= PerceptionRadius && Speed > 5.0f)
    {
        const float SpeedAlpha = FMath::Clamp(Speed / 600.0f, 0.15f, 1.0f);
        const float RunModifier = TargetPlayer->IsRunning() ? RunningMultiplier : 1.0f;
        const float Gain = MovementAwarenessPerSecond * SpeedAlpha * RunModifier *
            GetDistanceMultiplier(Distance) * GetTimeOfDayMultiplier() * DeltaTime;

        Awareness = FMath::Clamp(Awareness + Gain, 0.0f, 100.0f);

        if (LocationSampleCooldown <= 0.0f)
        {
            RememberPlayerLocation(TargetPlayer->GetActorLocation());
            LocationSampleCooldown = LocationSampleInterval;
            bReceivedPlayerStimulusThisFrame = true;
        }
        return;
    }

    Awareness = FMath::Max(0.0f, Awareness - AwarenessDecayPerSecond * DeltaTime);
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
    RememberPlayerLocation(WorldLocation);
    bReceivedPlayerStimulusThisFrame = true;
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
    RememberPlayerLocation(WorldLocation);
    bReceivedPlayerStimulusThisFrame = true;
}

void APOSTEntity::UpdateDecision(float DeltaTime)
{
    const bool bPlayerProtected = APOSTProtectionZone::IsActorProtected(this, TargetPlayer);

    if (!bPlayerProtected && bReceivedPlayerStimulusThisFrame && bHasLastKnownLocation)
    {
        bSearchAreaReached = false;
        SearchTimeRemaining = 0.0f;
        SearchMoveCooldown = 0.0f;

        if (Awareness >= HuntingThreshold)
        {
            SetEntityState(EPOSTEntityState::Hunting);
            MoveToward(LastKnownPlayerLocation);
            TryAttack();
            return;
        }

        if (Awareness >= InterestedThreshold)
        {
            bAttackCommitted = false;
            SetEntityState(EPOSTEntityState::Interested);
            MoveToward(LastKnownPlayerLocation);
            return;
        }
    }

    bAttackCommitted = false;

    if (bHasLastKnownLocation &&
        (EntityState == EPOSTEntityState::Hunting ||
         EntityState == EPOSTEntityState::Interested ||
         EntityState == EPOSTEntityState::Searching))
    {
        if (EntityState != EPOSTEntityState::Searching)
        {
            BeginSearch();
        }

        UpdateSearch(DeltaTime);
        return;
    }

    if (Awareness < InterestedThreshold && EntityState != EPOSTEntityState::Searching)
    {
        ForgetPlayer();
    }

    SetEntityState(EPOSTEntityState::Roaming);
    UpdateRoaming(DeltaTime);
}

void APOSTEntity::TryAttack()
{
    if (bAttackCommitted || !TargetPlayer || !bReceivedPlayerStimulusThisFrame ||
        APOSTProtectionZone::IsActorProtected(this, TargetPlayer))
    {
        return;
    }

    if (FVector::DistSquared(GetActorLocation(), TargetPlayer->GetActorLocation()) > FMath::Square(AttackDistance))
    {
        return;
    }

    bAttackCommitted = true;
    SetEntityState(EPOSTEntityState::Attacking);
    StopEntityMovement();
    OnAttackPlayer(TargetPlayer);

    if (APOSTCycleManager* CycleManager = Cast<APOSTCycleManager>(UGameplayStatics::GetActorOfClass(this, APOSTCycleManager::StaticClass())))
    {
        CycleManager->EndCycle(EPOSTDeathCause::Entity);
    }
}

void APOSTEntity::RememberPlayerLocation(const FVector& Location)
{
    LastKnownPlayerLocation = Location;
    bHasLastKnownLocation = true;
}

void APOSTEntity::BeginSearch()
{
    SetEntityState(EPOSTEntityState::Searching);
    bSearchAreaReached = false;
    SearchTimeRemaining = SearchDuration;
    SearchMoveCooldown = 0.0f;
    MoveToward(LastKnownPlayerLocation);
}

void APOSTEntity::UpdateSearch(float DeltaTime)
{
    const float DistanceToLastKnown = FVector::Dist2D(GetActorLocation(), LastKnownPlayerLocation);

    if (!bSearchAreaReached)
    {
        // MoveToLocation can finish before the pawn is inside our exact acceptance radius
        // because path following also accounts for the pawn/goal radii. Use a slightly
        // larger arrival threshold so search cannot get stuck at the last known point.
        const float ArrivalRadius = FMath::Max(SearchAcceptanceRadius * 2.0f, 200.0f);
        if (DistanceToLastKnown > ArrivalRadius)
        {
            return;
        }

        bSearchAreaReached = true;
        SearchTimeRemaining = SearchDuration;
        SearchMoveCooldown = 0.0f;
        StopEntityMovement();
    }

    SearchTimeRemaining -= DeltaTime;
    SearchMoveCooldown -= DeltaTime;

    if (SearchTimeRemaining <= 0.0f)
    {
        ForgetPlayer();
        Awareness = 0.0f;
        SetEntityState(EPOSTEntityState::Roaming);
        RoamingMoveCooldown = 0.0f;
        UpdateRoaming(DeltaTime);
        return;
    }

    if (SearchMoveCooldown > 0.0f)
    {
        return;
    }

    SearchMoveCooldown = SearchMoveInterval;

    if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        FNavLocation SearchPoint;
        if (NavSystem->GetRandomReachablePointInRadius(LastKnownPlayerLocation, SearchRadius, SearchPoint))
        {
            MoveToward(SearchPoint.Location, 60.0f);
        }
    }
}

void APOSTEntity::UpdateRoaming(float DeltaTime)
{
    RoamingMoveCooldown -= DeltaTime;
    if (RoamingMoveCooldown > 0.0f)
    {
        return;
    }

    RoamingMoveCooldown = RoamingMoveInterval;

    if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        FNavLocation RoamingPoint;
        if (NavSystem->GetRandomReachablePointInRadius(SpawnLocation, RoamingRadius, RoamingPoint))
        {
            MoveToward(RoamingPoint.Location, RoamingAcceptanceRadius);
        }
    }
}

void APOSTEntity::ForgetPlayer()
{
    bHasLastKnownLocation = false;
    LastKnownPlayerLocation = FVector::ZeroVector;
    bSearchAreaReached = false;
    SearchTimeRemaining = 0.0f;
    SearchMoveCooldown = 0.0f;
}

void APOSTEntity::MoveToward(const FVector& Location, float AcceptanceRadius)
{
    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        const float Radius = AcceptanceRadius >= 0.0f ? AcceptanceRadius : SearchAcceptanceRadius;
        AI->MoveToLocation(Location, Radius, true, true, false, true);
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
    ApplyMovementSpeedForState(EntityState);
    OnEntityStateChanged.Broadcast(OldState, EntityState);
}

void APOSTEntity::ApplyMovementSpeedForState(EPOSTEntityState State)
{
    UCharacterMovementComponent* Movement = GetCharacterMovement();
    if (!Movement)
    {
        return;
    }

    switch (State)
    {
    case EPOSTEntityState::Roaming:
        Movement->MaxWalkSpeed = RoamingSpeed;
        break;
    case EPOSTEntityState::Interested:
        Movement->MaxWalkSpeed = InterestedSpeed;
        break;
    case EPOSTEntityState::Searching:
        Movement->MaxWalkSpeed = SearchSpeed;
        break;
    case EPOSTEntityState::Hunting:
        Movement->MaxWalkSpeed = HuntingSpeed;
        break;
    default:
        break;
    }
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
