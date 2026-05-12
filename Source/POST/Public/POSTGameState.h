// Copyright (c) 2026 VAVnotDev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "POSTGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHourChanged, int32, NewHour);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinuteChanged, int32, NewMinute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeChanged, int32, Hours, int32, Minutes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDayStarted);

UCLASS()
class POST_API APOSTGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	APOSTGameState();

	UPROPERTY(BlueprintAssignable, Category = "POST Time")
	FOnHourChanged OnHourChanged;

	UPROPERTY(BlueprintAssignable, Category = "POST Time")
	FOnMinuteChanged OnMinuteChanged;

	UPROPERTY(BlueprintAssignable, Category = "POST Time")
	FOnNightStarted OnNightStarted;

	UPROPERTY(BlueprintAssignable, Category = "POST Time")
	FOnDayStarted OnDayStarted;

	UPROPERTY(BlueprintAssignable, Category = "POST Time")
	FOnTimeChanged OnTimeChanged;

	int32 GetHours() const { return Hours; }
	int32 GetMinutes() const { return Minutes; }
	int32 GetSeconds() const { return Seconds; }

	bool IsNight() const;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "POST Time")
	int32 Hours = 21;

	UPROPERTY(EditAnywhere, Category = "POST Time")
	int32 Minutes = 0;

	UPROPERTY(EditAnywhere, Category = "POST Time")
	int32 Seconds = 0;

	UPROPERTY(EditAnywhere, Category = "POST Time")
	float TimeScale = 60.0f;

	float AccumulateRealSeconds = 0.0f;

	bool bIsNight = false;

	void AddGameSecond();
	void CheckDayNightState();
};
