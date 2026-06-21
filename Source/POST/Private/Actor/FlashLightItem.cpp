// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "Actor/FlashLightItem.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFlashLightItem::AFlashLightItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	FlashlightStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashLightStaticMesh"));
	FlashlightStaticMesh->SetupAttachment(GetRootComponent());
	FlashlightStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlashlightStaticMesh->SetGenerateOverlapEvents(false);

	LightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("LightComponent"));
	LightComponent->SetupAttachment(GetRootComponent());
	LightComponent->SetIntensity(MinIntensity);
}

void AFlashLightItem::ToggleFlashLight()
{
	UE_LOG(LogTemp, Warning, TEXT("FlashLight: Call Toggle"));
	bIsFlashlightOn = !bIsFlashlightOn;

	if (ToggleSound)
		UGameplayStatics::PlaySoundAtLocation(this, ToggleSound, GetActorLocation());

	if (bIsFlashlightOn)
		setFlashLightEnabled();
	else
		setFlashLightDisabled();
}

void AFlashLightItem::setFlashLightEnabled()
{
	if (!LightComponent) return;
	
	LightComponent->SetIntensity(MaxIntensity);		
}

void AFlashLightItem::setFlashLightDisabled()
{
	LightComponent->SetIntensity(MinIntensity);
}

// Called when the game starts or when spawned
void AFlashLightItem::BeginPlay()
{
	Super::BeginPlay();
}


