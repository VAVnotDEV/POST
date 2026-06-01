// Copyright (c) 2026 VAVnotDev. All Rights Reserved.


#include "POSTTemperatureDisplay.h"
#include "POSTWeatherManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "POSTLog.h"

// Sets default values
APOSTTemperatureDisplay::APOSTTemperatureDisplay()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DisplayMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayComponent"));
	DisplayMeshComponent->SetupAttachment(GetRootComponent());

	TemperatureTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TemperatureText"));
	TemperatureTextComponent->SetupAttachment(DisplayMeshComponent);
}

void APOSTTemperatureDisplay::HandleTemperatureChanged(float NewTemperature)
{
	UE_LOG(LogPOST, Warning, TEXT("TempDisplay: Handle Call %f"), NewTemperature)
	TemperatureTextComponent->SetText(FText::FromString(FString::Printf(TEXT(" %+.0f°C"), NewTemperature)));
}

// Called when the game starts or when spawned
void APOSTTemperatureDisplay::BeginPlay()
{
	Super::BeginPlay();

	WeatherManager = Cast<APOSTWeatherManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APOSTWeatherManager::StaticClass()));
	
	if (WeatherManager)
	{
		UE_LOG(LogPOST, Warning, TEXT("TempDisplay: WeatherManager-Have"))
		WeatherManager->OnTemperatureChanged.AddUObject(this, &APOSTTemperatureDisplay::HandleTemperatureChanged);
	}
}

// Called every frame
void APOSTTemperatureDisplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

