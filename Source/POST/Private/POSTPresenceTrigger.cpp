// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "POSTPresenceTrigger.h"
#include "Components/BoxComponent.h"
#include "Gameplay/POSTGameDirector.h"
#include "Kismet/GameplayStatics.h"
#include "Player/POSTCharacter.h"	
#include "POSTLog.h"

// Sets default values
APOSTPresenceTrigger::APOSTPresenceTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}


void APOSTPresenceTrigger::OnTriggeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogPOST, Warning, TEXT("TRIGGER FIRED: %s"), *GetNameSafe(OtherActor));
	if (!Director)
	{
		UE_LOG(LogPOST, Error, TEXT("Presence Director NOT FOUND"));
		return;
	}

	if (Cast<APOSTCharacter>(OtherActor))
	{
		UE_LOG(LogPOST, Warning, TEXT("Presence: Player entered trigger"));
		Director->StartPresenceEncounter();
	}
}


void APOSTPresenceTrigger::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Director) return;

	if (Cast<APOSTCharacter>(OtherActor))
	{
		UE_LOG(LogPOST, Warning, TEXT("Presence: Player left trigger"));
		Director->StopPresenceEncounter();
	}
}

// Called when the game starts or when spawned
void APOSTPresenceTrigger::BeginPlay()
{
	Director = Cast<APOSTGameDirector>(UGameplayStatics::GetActorOfClass(this, APOSTGameDirector::StaticClass()));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APOSTPresenceTrigger::OnTriggeBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APOSTPresenceTrigger::OnTriggerEndOverlap);


	Super::BeginPlay();
	
}