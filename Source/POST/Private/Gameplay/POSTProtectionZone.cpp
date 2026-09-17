#include "Gameplay/POSTProtectionZone.h"

#include "Components/BoxComponent.h"
#include "EngineUtils.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Null.h"

APOSTProtectionZone::APOSTProtectionZone()
{
    PrimaryActorTick.bCanEverTick = false;

    ProtectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProtectionBox"));
    SetRootComponent(ProtectionBox);
    ProtectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ProtectionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    ProtectionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    ProtectionBox->SetGenerateOverlapEvents(true);

    NavigationBlocker = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavigationBlocker"));
    NavigationBlocker->SetAreaClass(UNavArea_Null::StaticClass());
}

bool APOSTProtectionZone::IsActorInside(const AActor* Actor) const
{
    return IsValid(Actor) && ProtectionBox && ProtectionBox->IsOverlappingActor(Actor);
}

bool APOSTProtectionZone::IsActorProtected(const UObject* WorldContextObject, const AActor* Actor)
{
    if (!WorldContextObject || !Actor)
    {
        return false;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        return false;
    }

    for (TActorIterator<APOSTProtectionZone> It(World); It; ++It)
    {
        if (It->IsActorInside(Actor))
        {
            return true;
        }
    }

    return false;
}
