// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#include "Actor/FlashLightItem.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

AFlashLightItem::AFlashLightItem()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    FlashlightStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashLightStaticMesh"));
    FlashlightStaticMesh->SetupAttachment(GetRootComponent());
    FlashlightStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    FlashlightStaticMesh->SetGenerateOverlapEvents(false);

    LightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("LightComponent"));
    LightComponent->SetupAttachment(GetRootComponent());
    LightComponent->SetIntensity(0.0f);
}

void AFlashLightItem::BeginPlay()
{
    Super::BeginPlay();
    SetFlashLightEnabled(bStartEnabled, false);
}

void AFlashLightItem::ToggleFlashLight()
{
    SetFlashLightEnabled(!bIsFlashlightOn, true);
}

void AFlashLightItem::SetFlashLightEnabled(bool bEnabled, bool bPlaySound)
{
    if (!LightComponent) return;
    if (bIsFlashlightOn == bEnabled && LightComponent->Intensity == (bEnabled ? MaxIntensity : 0.0f)) return;

    bIsFlashlightOn = bEnabled;
    LightComponent->SetIntensity(bIsFlashlightOn ? MaxIntensity : 0.0f);

    if (bPlaySound && ToggleSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ToggleSound, GetActorLocation());
    }
}
