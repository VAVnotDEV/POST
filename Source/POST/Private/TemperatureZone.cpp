// Fill out your copyright notice in the Description page of Project Settings.


#include "TemperatureZone.h"
#include "Components/BoxComponent.h"
#include "Player/POSTCharacter.h"
#include "Components/POSTTemperatureComponent.h"
#include "POSTLog.h"

// Sets default values
ATemperatureZone::ATemperatureZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SetRootComponent(Box);

	//Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//Box->SetCollisionObjectType(ECC_WorldDynamic);
	//Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	//Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	//Box->SetGenerateOverlapEvents(true);

	//TArray<AActor*> OverlappingActors;
	//Box->GetOverlappingActors(OverlappingActors, APOSTCharacter::StaticClass());

	//for (AActor* Actor : OverlappingActors)
	//{
	//	APOSTCharacter* Character = Cast<APOSTCharacter>(Actor);
	//	if (!Character) continue;

	//	UPOSTTemperatureComponent* TempComp = Character->GetTemperatureComponent();
	//	if (!TempComp) continue;

	//	TempComp->SetInWarmZone(true);

	//	UE_LOG(LogPOST, Display, TEXT("[Temperature] Player already inside warm zone"));
	//}
}

// Called when the game starts or when spawned
void ATemperatureZone::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &ATemperatureZone::OnBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ATemperatureZone::OnEndOverlap);
}

void ATemperatureZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogPOST, Warning, TEXT("Enter warm zone"));
	APOSTCharacter* Player = Cast<APOSTCharacter>(OtherActor);
	if (!Player) return;

	UPOSTTemperatureComponent* TempComp = Player->GetTemperatureComponent();
	if (!TempComp) return;

	TempComp->SetInWarmZone(true);

	

}

void ATemperatureZone::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogPOST, Warning, TEXT("Exit warm zone"));

	APOSTCharacter* Player = Cast<APOSTCharacter>(OtherActor);
	if (!Player) return;

	UPOSTTemperatureComponent* TempComp = Player->GetTemperatureComponent();
	if (!TempComp) return;

	TempComp->SetInWarmZone(false);
}


