#include "Gameplay/POSTGenerator.h"
#include "Gameplay/POSTFuelContainer.h"
#include "Gameplay/POSTGameDirector.h"
#include "POSTGameState.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

APOSTGenerator::APOSTGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void APOSTGenerator::BeginPlay()
{
    Super::BeginPlay();
    Fuel = FMath::Clamp(Fuel, 0.0f, FuelCapacity);
    Condition = FMath::Clamp(Condition, 0.0f, 1.0f);

    CachedGameState = GetWorld() ? GetWorld()->GetGameState<APOSTGameState>() : nullptr;
    if (CachedGameState)
    {
        CachedGameState->OnMinuteChanged.AddDynamic(this, &APOSTGenerator::HandleGameMinuteChanged);
    }

    APOSTGameDirector* Director = Cast<APOSTGameDirector>(UGameplayStatics::GetActorOfClass(this, APOSTGameDirector::StaticClass()));
    if (Director) ApplyReliabilityMultiplier(Director->GetGeneratorReliabilityMultiplier());

    OnFuelChanged.Broadcast(Fuel);
    OnConditionChanged.Broadcast(Condition);
    OnStateChanged.Broadcast(State);
}

void APOSTGenerator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (CachedGameState)
    {
        CachedGameState->OnMinuteChanged.RemoveDynamic(this, &APOSTGenerator::HandleGameMinuteChanged);
    }
    Super::EndPlay(EndPlayReason);
}

void APOSTGenerator::HandleGameMinuteChanged(int32 NewMinute)
{
    SimulateGameMinute();
}

void APOSTGenerator::SimulateGameMinute()
{
    if (State != EPOSTGeneratorState::Running) return;

    Fuel = FMath::Max(0.0f, Fuel - FuelConsumptionPerGameMinute);
    Condition = FMath::Max(0.0f, Condition - WearPerGameMinute / FMath::Max(0.1f, ReliabilityMultiplier));
    OnFuelChanged.Broadcast(Fuel);
    OnConditionChanged.Broadcast(Condition);

    if (Fuel <= KINDA_SMALL_NUMBER)
    {
        StopGenerator();
        return;
    }

    const float ConditionFactor = 1.0f + (1.0f - Condition);
    const float FailureChance = RandomFailureChancePerGameMinute * ConditionFactor / FMath::Max(0.1f, ReliabilityMultiplier);
    if (Condition <= KINDA_SMALL_NUMBER || FMath::FRand() < FailureChance)
    {
        BreakGenerator();
    }
}

bool APOSTGenerator::StartGenerator()
{
    if (State == EPOSTGeneratorState::Broken || Fuel <= KINDA_SMALL_NUMBER) return false;
    SetState(EPOSTGeneratorState::Running);
    OnGeneratorStarted();
    return true;
}

void APOSTGenerator::StopGenerator()
{
    if (State == EPOSTGeneratorState::Stopped) return;
    SetState(EPOSTGeneratorState::Stopped);
    OnGeneratorStopped();
}

void APOSTGenerator::BreakGenerator()
{
    if (State == EPOSTGeneratorState::Broken) return;
    SetState(EPOSTGeneratorState::Broken);
    OnGeneratorBroken();
}

void APOSTGenerator::RepairGenerator(float RepairAmount)
{
    Condition = FMath::Clamp(Condition + FMath::Max(0.0f, RepairAmount), 0.0f, 1.0f);
    OnConditionChanged.Broadcast(Condition);
    if (State == EPOSTGeneratorState::Broken && Condition >= 0.25f)
    {
        SetState(EPOSTGeneratorState::Stopped);
    }
}

float APOSTGenerator::RefuelFrom(APOSTFuelContainer* Container, float RequestedAmount)
{
    if (!IsValid(Container)) return 0.0f;
    const float Needed = FMath::Min(FMath::Max(0.0f, RequestedAmount), FuelCapacity - Fuel);
    const float Removed = Container->RemoveFuel(Needed);
    return AddFuel(Removed);
}

float APOSTGenerator::AddFuel(float Amount)
{
    const float Accepted = FMath::Clamp(Amount, 0.0f, FuelCapacity - Fuel);
    Fuel += Accepted;
    if (Accepted > 0.0f) OnFuelChanged.Broadcast(Fuel);
    return Accepted;
}

void APOSTGenerator::ApplyReliabilityMultiplier(float Multiplier)
{
    ReliabilityMultiplier = FMath::Max(0.1f, Multiplier);
}

void APOSTGenerator::SetState(EPOSTGeneratorState NewState)
{
    if (State == NewState) return;
    State = NewState;
    OnStateChanged.Broadcast(State);
}

bool APOSTGenerator::CanInteract_Implementation(AActor* Interactor) const
{
    return IsValid(Interactor);
}

FText APOSTGenerator::GetInteractText_Implementation(AActor* Interactor) const
{
    if (State == EPOSTGeneratorState::Broken) return FText::FromString(TEXT("Починить генератор"));
    if (State == EPOSTGeneratorState::Running) return FText::FromString(TEXT("Заглушить генератор"));
    if (Fuel <= KINDA_SMALL_NUMBER && DefaultFuelSource && DefaultFuelSource->GetFuel() > 0.0f) return FText::FromString(TEXT("Заправить генератор"));
    if (Fuel <= KINDA_SMALL_NUMBER) return FText::FromString(TEXT("Нет топлива"));
    return FText::FromString(TEXT("Запустить генератор"));
}

void APOSTGenerator::Interact_Implementation(AActor* Interactor)
{
    if (State == EPOSTGeneratorState::Broken)
    {
        RepairGenerator(RepairAmountPerInteraction);
        return;
    }
    if (State == EPOSTGeneratorState::Running)
    {
        StopGenerator();
        return;
    }
    if (Fuel <= KINDA_SMALL_NUMBER && DefaultFuelSource)
    {
        RefuelFrom(DefaultFuelSource, RefuelAmountPerInteraction);
        return;
    }
    StartGenerator();
}
