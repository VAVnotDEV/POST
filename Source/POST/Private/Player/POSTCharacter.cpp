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
#include "Gameplay/POSTCarryableActor.h"
#include "Gameplay/POSTGameDirector.h"
#include "Kismet/GameplayStatics.h"

APOSTCharacter::APOSTCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<UPOSTMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

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

}

void APOSTCharacter::BeginPlay()
{
    Super::BeginPlay();

    SpawnFlashlight();
    TemperatureComponent->OnPlayerFrozen.AddDynamic(this, &APOSTCharacter::HandleFrozen);
}

void APOSTCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateStaminaUsage();
}

void APOSTCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    DropCarriedActor();
    Super::EndPlay(EndPlayReason);
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
    UpdateStaminaUsage();
}

void APOSTCharacter::OnStopRunning()
{
    bWantsToRun = false;
    UpdateStaminaUsage();
}

void APOSTCharacter::UpdateStaminaUsage()
{
    if (!StaminaComponent)
    {
        return;
    }

    if (IsRunning())
    {
        StaminaComponent->StartSpendStamina();
    }
    else
    {
        StaminaComponent->StopSpendStamina();
    }
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
    if (!IsValid(Actor) || IsValid(CarriedActor) || Actor->IsCarried())
    {
        return false;
    }

    if (!Actor->AttachToCharacter(this))
    {
        return false;
    }

    CarriedActor = Actor;
    return true;
}

void APOSTCharacter::DropCarriedActor()
{
    if (!IsValid(CarriedActor))
    {
        CarriedActor = nullptr;
        return;
    }

    APOSTCarryableActor* ActorToDrop = CarriedActor;
    CarriedActor = nullptr;
    ActorToDrop->Drop();
}

void APOSTCharacter::NotifyCarriedActorReleased(APOSTCarryableActor* Actor)
{
    if (CarriedActor == Actor)
    {
        CarriedActor = nullptr;
    }
}


void APOSTCharacter::HandleFrozen()
{
    APOSTGameDirector* Director = Cast<APOSTGameDirector>(
        UGameplayStatics::GetActorOfClass(this, APOSTGameDirector::StaticClass()));

    if (!Director)
    {
        UE_LOG(LogTemp, Error, TEXT("Player froze, but POSTGameDirector was not found in the level."));
        return;
    }

    Director->RegisterDeath(EPOSTDeathCause::Cold);
}
