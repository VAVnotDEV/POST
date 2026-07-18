#include "Gameplay/POSTGenerator.h"
#include "Gameplay/POSTFuelContainer.h"
#include "Components/SceneComponent.h"

APOSTGenerator::APOSTGenerator()
{
    PrimaryActorTick.bCanEverTick = true;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}
void APOSTGenerator::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (State != EPOSTGeneratorState::Running) return;
    RuntimeAccumulator += DeltaSeconds;
    while (RuntimeAccumulator >= 60.0f) { RuntimeAccumulator -= 60.0f; SimulateMinute(); }
}
void APOSTGenerator::SimulateMinute()
{
    Fuel = FMath::Max(0.0f, Fuel - FuelConsumptionPerMinute);
    Condition = FMath::Max(0.0f, Condition - WearPerMinute / FMath::Max(0.1f, ReliabilityMultiplier));
    OnFuelChanged.Broadcast(Fuel);
    if (Fuel <= 0.0f) { StopGenerator(); return; }
    const float FailureChance = RandomFailureChancePerMinute * (1.0f + (1.0f - Condition)) / FMath::Max(0.1f, ReliabilityMultiplier);
    if (Condition <= 0.0f || FMath::FRand() < FailureChance) BreakGenerator();
}
bool APOSTGenerator::StartGenerator()
{
    if (State == EPOSTGeneratorState::Broken || Fuel <= 0.0f) return false;
    SetState(EPOSTGeneratorState::Running); OnGeneratorStarted(); return true;
}
void APOSTGenerator::StopGenerator()
{
    if (State == EPOSTGeneratorState::Stopped) return;
    SetState(EPOSTGeneratorState::Stopped); OnGeneratorStopped();
}
void APOSTGenerator::BreakGenerator()
{
    SetState(EPOSTGeneratorState::Broken); OnGeneratorBroken();
}
void APOSTGenerator::RepairGenerator(float RepairAmount)
{
    Condition = FMath::Clamp(Condition + FMath::Max(0.0f, RepairAmount), 0.0f, 1.0f);
    if (State == EPOSTGeneratorState::Broken && Condition > 0.25f) SetState(EPOSTGeneratorState::Stopped);
}
float APOSTGenerator::RefuelFrom(APOSTFuelContainer* Container, float RequestedAmount)
{
    if (!Container) return 0.0f;
    const float Need = FMath::Min(FMath::Max(0.0f, RequestedAmount), FuelCapacity - Fuel);
    const float Removed = Container->RemoveFuel(Need);
    Fuel += Removed; OnFuelChanged.Broadcast(Fuel); return Removed;
}
void APOSTGenerator::SetState(EPOSTGeneratorState NewState)
{
    if (State == NewState) return; State = NewState; OnStateChanged.Broadcast(State);
}
bool APOSTGenerator::CanInteract_Implementation(AActor* Interactor) const { return IsValid(Interactor); }
FText APOSTGenerator::GetInteractText_Implementation(AActor* Interactor) const
{
    if (State == EPOSTGeneratorState::Broken) return FText::FromString(TEXT("Починить генератор"));
    return State == EPOSTGeneratorState::Running ? FText::FromString(TEXT("Заглушить генератор")) : FText::FromString(TEXT("Запустить генератор"));
}
void APOSTGenerator::Interact_Implementation(AActor* Interactor)
{
    if (State == EPOSTGeneratorState::Running) StopGenerator(); else StartGenerator();
}
