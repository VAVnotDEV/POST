// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#include "Player/POSTCharacter.h"

#include "Actor/FlashLightItem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/POSTFootstepComponent.h"
#include "Components/POSTInteractionComponent.h"
#include "Components/POSTMovementComponent.h"
#include "Components/POSTRadioComponent.h"
#include "Components/POSTStaminaComponent.h"
#include "Components/POSTTemperatureComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "Gameplay/POSTCarryableActor.h"

APOSTCharacter::APOSTCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<UPOSTMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = false;

    GetCapsuleComponent()->InitCapsuleSize(42.0f, 88.0f);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(GetCapsuleComponent());
    Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
    Camera->bUsePawnControlRotation = true;

    FlashlightAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FlashLightAttachPoint"));
    FlashlightAttachPoint->SetupAttachment(Camera);

    CarryPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CarryPoint"));
    CarryPoint->SetupAttachment(Camera);
    CarryPoint->SetRelativeLocation(FVector(120.0f, 0.0f, -20.0f));

    TemperatureComponent = CreateDefaultSubobject<UPOSTTemperatureComponent>(TEXT("TemperatureComponent"));
    StaminaComponent = CreateDefaultSubobject<UPOSTStaminaComponent>(TEXT("StaminaComponent"));
    RadioComponent = CreateDefaultSubobject<UPOSTRadioComponent>(TEXT("RadioComponent"));
    InteractionComponent = CreateDefaultSubobject<UPOSTInteractionComponent>(TEXT("InteractionComponent"));
    FootstepComponent = CreateDefaultSubobject<UPOSTFootstepComponent>(TEXT("FootstepComponent"));

    StaminaTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StaminaTextComponent"));
    StaminaTextComponent->SetupAttachment(GetRootComponent());

    TemperatureTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TemperatureTextComponent"));
    TemperatureTextComponent->SetupAttachment(GetRootComponent());
}

void APOSTCharacter::BeginPlay()
{
    Super::BeginPlay();

    SpawnFlashlight();
    OnBodyTemperatureChanged(TemperatureComponent->GetCurrentTemperature());
    OnStaminaChanged(StaminaComponent->GetCurrentStamina());

    TemperatureComponent->OnBodyTemperatureChanged.AddDynamic(this, &APOSTCharacter::OnBodyTemperatureChanged);
    TemperatureComponent->OnPlayerFrozen.AddDynamic(this, &APOSTCharacter::HandleFrozen);
    StaminaComponent->OnStaminaChanged.AddDynamic(this, &APOSTCharacter::OnStaminaChanged);
}

void APOSTCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APOSTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APOSTCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APOSTCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APOSTCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APOSTCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAction(TEXT("Flashlight"), IE_Pressed, this, &APOSTCharacter::ToggleFlashlight);
    PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &APOSTCharacter::OnStartRunning);
    PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &APOSTCharacter::OnStopRunning);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &APOSTCharacter::TryInteract);
    PlayerInputComponent->BindAction(TEXT("Drop"), IE_Pressed, this, &APOSTCharacter::DropCarriedActor);
}

void APOSTCharacter::MoveForward(float Amount)
{
    bIsMovingForward = Amount > 0.0f;
    AddMovementInput(GetActorForwardVector(), Amount);
}

void APOSTCharacter::MoveRight(float Amount)
{
    AddMovementInput(GetActorRightVector(), Amount);
}

void APOSTCharacter::SpawnFlashlight()
{
    if (!FlashlightClass || !GetWorld())
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;

    FlashlightActor = GetWorld()->SpawnActor<AFlashLightItem>(
        FlashlightClass,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        SpawnParams);

    if (FlashlightActor)
    {
        FlashlightActor->AttachToComponent(
            FlashlightAttachPoint,
            FAttachmentTransformRules::SnapToTargetIncludingScale);
    }
}

void APOSTCharacter::ToggleFlashlight()
{
    if (FlashlightActor)
    {
        FlashlightActor->ToggleFlashLight();
    }
}

bool APOSTCharacter::IsRunning() const
{
    return bWantsToRun && bIsMovingForward && !GetVelocity().IsNearlyZero() && StaminaComponent->CanRun();
}

void APOSTCharacter::OnStartRunning()
{
    bWantsToRun = true;
    StaminaComponent->StartSpendStamina();
}

void APOSTCharacter::OnStopRunning()
{
    bWantsToRun = false;
    StaminaComponent->StopSpendStamina();
}

void APOSTCharacter::TryInteract()
{
    if (InteractionComponent)
    {
        InteractionComponent->TryInteract();
    }
}

bool APOSTCharacter::TryCarry(APOSTCarryableActor* Actor)
{
    if (!Actor || CarriedActor)
    {
        return false;
    }

    CarriedActor = Actor;
    Actor->AttachToCharacter(this);
    return true;
}

void APOSTCharacter::DropCarriedActor()
{
    if (!CarriedActor)
    {
        return;
    }

    APOSTCarryableActor* ActorToDrop = CarriedActor;
    CarriedActor = nullptr;
    ActorToDrop->Drop();
}

void APOSTCharacter::OnBodyTemperatureChanged(float NewTemp)
{
    if (TemperatureTextComponent)
    {
        TemperatureTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Temp: %.0f"), NewTemp)));
    }
}

void APOSTCharacter::OnStaminaChanged(float NewStamina)
{
    if (StaminaTextComponent)
    {
        StaminaTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Stamina: %.0f"), NewStamina)));
    }
}

void APOSTCharacter::HandleFrozen()
{
    UE_LOG(LogTemp, Warning, TEXT("Player frozen. Director should register a cold reboot."));
}
