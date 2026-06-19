// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#include "Player/POSTCharacter.h"
#include "Interfaces/Interactable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actor/FlashLightItem.h"
#include "Components/POSTMovementComponent.h"
#include "Components/POSTTemperatureComponent.h"
#include "Components/POSTStaminaComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/POSTEntityAudioComponent.h"
#include "Components/POSTInteractionComponent.h"
#include "POSTLog.h"



//DEFINE_LOG_CATEGORY_STATIC(LogPOST, Display, All)

// Sets default values
APOSTCharacter::APOSTCharacter(const FObjectInitializer& ObjInit) 
	: Super(ObjInit.SetDefaultSubobjectClass<UPOSTMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 88.f);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	Camera->bUsePawnControlRotation = true;

	FlashlightAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FlashLightAttachPoint"));
	FlashlightAttachPoint->SetupAttachment(Camera);
	
	FlashlightActor = nullptr;

	TemperatureComponent = CreateDefaultSubobject<UPOSTTemperatureComponent>(TEXT("TemperatureComponent"));
	StaminaComponent = CreateDefaultSubobject<UPOSTStaminaComponent>(TEXT("StaminaComponent"));

	StaminaTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StaminaTextComponent"));
	StaminaTextComponent->SetupAttachment(GetRootComponent());

	TemperaturaTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TemperaturaTextComponent"));
	TemperaturaTextComponent->SetupAttachment(GetRootComponent());

	EntityAudioComponent = CreateDefaultSubobject<UPOSTEntityAudioComponent>(TEXT("EntityAudioComponent"));

	InteractionComponent = CreateDefaultSubobject<UPOSTInteractionComponent>(TEXT("InteractionComponent"));
}

// Called when the game starts or when spawned
void APOSTCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnFlashlight();
	OnBodyTemperatureChanged(TemperatureComponent->GetCurrentTemperature());
	TemperatureComponent->OnBodyTemperatureChanged.AddUObject(this, &APOSTCharacter::OnBodyTemperatureChanged);
}

// Called every frame
void APOSTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const auto Stamina = StaminaComponent->GetCurrentStamina();
	StaminaTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Stamina: %.0f"), Stamina)));
	
}

// Called to bind functionality to input
void APOSTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APOSTCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APOSTCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APOSTCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APOSTCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Flashlight", IE_Pressed, this, &APOSTCharacter::ToggleFlashlight);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APOSTCharacter::OnStartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APOSTCharacter::OnStopRunning);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APOSTCharacter::TryInteract);
	
}

void APOSTCharacter::MoveForward(float Amount)
{
	IsMovingForward = Amount > 0.0f;
	AddMovementInput(GetActorForwardVector(), Amount);
}

void APOSTCharacter::MoveRight(float Amount)
{
	
	AddMovementInput(GetActorRightVector(), Amount);
}

void APOSTCharacter::SpawnFlashlight()
{
	if (!FlashlightClass) { return; }

	if (!GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	FlashlightActor = GetWorld()->SpawnActor<AFlashLightItem>(
		FlashlightClass, 
		FVector::ZeroVector, 
		FRotator::ZeroRotator, 
		SpawnParams);

	if (!FlashlightActor) { return; }

	FlashlightActor->AttachToComponent(FlashlightAttachPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void APOSTCharacter::ToggleFlashlight()
{
	if (!FlashlightActor) { return; }

	FlashlightActor->ToggleFlashLight();
}

bool APOSTCharacter::IsRunning() const
{
	return WantsToRun && IsMovingForward && !GetVelocity().IsZero() && StaminaComponent->CanRun();
}

UPOSTTemperatureComponent* APOSTCharacter::GetTemperatureComponent() const
{
	return TemperatureComponent;
}

UPOSTStaminaComponent* APOSTCharacter::GetStaminaComponent() const
{
	return StaminaComponent;
}

void APOSTCharacter::OnStartRunning()
{
	WantsToRun = true;
}

void APOSTCharacter::OnStopRunning()
{
	WantsToRun = false;
}


void APOSTCharacter::TryInteract()
{
	if (!InteractionComponent) return;

	InteractionComponent->TryInteract();
}

void APOSTCharacter::OnBodyTemperatureChanged(float NewTemp)
{
	UE_LOG(LogPOST, Warning, TEXT("Char: OnBodyTempChang Call"))
	TemperaturaTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Temp: %.0f"), NewTemp)));
}
