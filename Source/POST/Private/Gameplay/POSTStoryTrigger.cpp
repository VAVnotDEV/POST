#include "Gameplay/POSTStoryTrigger.h"
#include "Gameplay/POSTGameDirector.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

APOSTStoryTrigger::APOSTStoryTrigger()
{
    PrimaryActorTick.bCanEverTick = false;
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->SetCollisionProfileName(TEXT("Trigger"));
}

void APOSTStoryTrigger::BeginPlay()
{
    Super::BeginPlay();
    if (!Director)
    {
        Director = Cast<APOSTGameDirector>(UGameplayStatics::GetActorOfClass(this, APOSTGameDirector::StaticClass()));
    }
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &APOSTStoryTrigger::HandleOverlap);
}

void APOSTStoryTrigger::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!Director || !OtherActor || !Cast<APawn>(OtherActor)) return;
    if (Director->AdvanceStoryStage(RequiredStage, NextStage))
    {
        OnStoryTriggered(OtherActor);
        if (bDisableAfterTrigger) Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
