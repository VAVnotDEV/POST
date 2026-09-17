#include "Gameplay/POSTEntity.h"

#include "AIController.h"
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
    TargetPlayer = Cast<APOSTCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
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
        RememberPlayerLocation(TargetPlayer->GetActorLocation());
        bReceivedPlayerStimulusThisFrame = true;
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

    if (!bPlayerProtected && bReceivedPlayerStimulusThisFrame && Awareness >= HuntingThreshold)
    {
        SearchTimeRemaining = 0.0f;
        SearchMoveCooldown = 0.0f;
        SetEntityState(EPOSTEntityState::Hunting);
        MoveToward(LastKnownPlayerLocation);
        TryAttack();
        return;
    }

    bAttackCommitted = false;

    if (bHasLastKnownLocation)
    {
        if (EntityState != EPOSTEntityState::Searching)
        {
            BeginSearch();
        }

        UpdateSearch(DeltaTime);
        return;
    }

    SetEntityState(EPOSTEntityState::Roaming);
    StopEntityMovement();
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
    SearchTimeRemaining = SearchDuration;
    SearchMoveCooldown = 0.0f;
    MoveToward(LastKnownPlayerLocation);
}

void APOSTEntity::UpdateSearch(float DeltaTime)
{
    SearchTimeRemaining -= DeltaTime;
    SearchMoveCooldown -= DeltaTime;

    if (SearchTimeRemaining <= 0.0f || Awareness <= KINDA_SMALL_NUMBER)
    {
        ForgetPlayer();
        SetEntityState(EPOSTEntityState::Roaming);
        StopEntityMovement();
        return;
    }

    const float DistanceToLastKnown = FVector::Dist2D(GetActorLocation(), LastKnownPlayerLocation);
    if (DistanceToLastKnown > SearchAcceptanceRadius)
    {
        MoveToward(LastKnownPlayerLocation);
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
            MoveToward(SearchPoint.Location);
        }
    }
}

void APOSTEntity::ForgetPlayer()
{
    bHasLastKnownLocation = false;
    LastKnownPlayerLocation = FVector::ZeroVector;
    SearchTimeRemaining = 0.0f;
    SearchMoveCooldown = 0.0f;
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
