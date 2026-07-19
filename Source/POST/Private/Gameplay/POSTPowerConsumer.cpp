#include "Gameplay/POSTPowerConsumer.h"
#include "Gameplay/POSTGenerator.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

APOSTPowerConsumer::APOSTPowerConsumer()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void APOSTPowerConsumer::BeginPlay()
{
    Super::BeginPlay();
    if (!Generator && bAutoFindGenerator)
    {
        Generator = Cast<APOSTGenerator>(UGameplayStatics::GetActorOfClass(this, APOSTGenerator::StaticClass()));
    }
    BindGenerator();
    RefreshPower();
}

void APOSTPowerConsumer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindGenerator();
    Super::EndPlay(EndPlayReason);
}

void APOSTPowerConsumer::SetEnabled(bool bNewEnabled)
{
    bEnabled = bNewEnabled;
    RefreshPower();
}

void APOSTPowerConsumer::SetGenerator(APOSTGenerator* NewGenerator)
{
    if (Generator == NewGenerator) return;
    UnbindGenerator();
    Generator = NewGenerator;
    BindGenerator();
    RefreshPower();
}

void APOSTPowerConsumer::BindGenerator()
{
    if (!Generator) return;
    Generator->OnStateChanged.AddDynamic(this, &APOSTPowerConsumer::HandleGeneratorStateChanged);
    Generator->OnFuelChanged.AddDynamic(this, &APOSTPowerConsumer::HandleGeneratorFuelChanged);
}

void APOSTPowerConsumer::UnbindGenerator()
{
    if (!Generator) return;
    Generator->OnStateChanged.RemoveDynamic(this, &APOSTPowerConsumer::HandleGeneratorStateChanged);
    Generator->OnFuelChanged.RemoveDynamic(this, &APOSTPowerConsumer::HandleGeneratorFuelChanged);
}

void APOSTPowerConsumer::HandleGeneratorStateChanged(EPOSTGeneratorState NewState) { RefreshPower(); }
void APOSTPowerConsumer::HandleGeneratorFuelChanged(float NewFuel) { RefreshPower(); }

void APOSTPowerConsumer::RefreshPower()
{
    const bool bNewPowered = bEnabled && IsValid(Generator) && Generator->HasPower();
    if (bNewPowered == bPowered) return;
    bPowered = bNewPowered;
    OnPowerStateChanged.Broadcast(bPowered);
    OnPowerChanged(bPowered);
}
