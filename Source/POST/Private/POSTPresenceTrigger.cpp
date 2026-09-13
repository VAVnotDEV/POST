// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "POSTPresenceTrigger.h"
#include "Components/BoxComponent.h"
#include "Gameplay/POSTGameDirector.h"
#include "Kismet/GameplayStatics.h"
#include "Player/POSTCharacter.h"	

// Sets default values
APOSTPresenceTrigger::APOSTPresenceTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

UFUNCTION()
void APOSTPresenceTrigger::OnTriggeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Director) return;

	if (Cast<APOSTCharacter>(OtherActor))
		Director->StartPresenceEncounter();
}

UFUNCTION()
void APOSTPresenceTrigger::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Director) return;

	if (Cast<APOSTGameDirector>(OtherActor))
		Director->StopPresenceEncounter();
}

// Called when the game starts or when spawned
void APOSTPresenceTrigger::BeginPlay()
{
	Director = Cast<APOSTGameDirector>(UGameplayStatics::GetActorOfClass(this, APOSTGameDirector::StaticClass()));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APOSTPresenceTrigger::OnTriggeBeginOverlap);

	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APOSTPresenceTrigger::OnTriggerEndOverlap);


	Super::BeginPlay();
	
}

// Called every frame
void APOSTPresenceTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

