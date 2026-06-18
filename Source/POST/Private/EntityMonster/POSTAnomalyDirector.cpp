// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "EntityMonster/POSTAnomalyDirector.h"
#include "Kismet/GameplayStatics.h"
#include "EntityMonster/POSTEntityPresence.h"

// Sets default values
APOSTAnomalyDirector::APOSTAnomalyDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APOSTAnomalyDirector::BeginPlay()
{
	Super::BeginPlay();
	
    EntityPresence = Cast<APOSTEntityPresence>(
        UGameplayStatics::GetActorOfClass(GetWorld(), APOSTEntityPresence::StaticClass())
    );

    GetWorldTimerManager().SetTimer(
        AnomalyTimerHandle,
        this,
        &APOSTAnomalyDirector::TryActivateAnomaly,
        5.0f,
        true
    );
}

void APOSTAnomalyDirector::TryActivateAnomaly()
{
    if (!EntityPresence) return;

    const float Threat = EntityPresence->GetThreatLevel();

    if (Threat < 0.4f) return;

    const float Roll = FMath::FRand();

    if (Roll < Threat)
    {
        UE_LOG(LogTemp, Warning, TEXT("Anomaly activated. Threat: %f"), Threat);

        // Потом здесь будет выбор APOSTAnomalyBase
    }
}


