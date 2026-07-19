#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTTemperatureComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTBodyTemperatureChanged, float, NewTemperature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPOSTPlayerFrozen);

UCLASS(ClassGroup=(POST), meta=(BlueprintSpawnableComponent))
class POST_API UPOSTTemperatureComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPOSTTemperatureComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="POST|Temperature") void SetInWarmZone(bool bIsInZone);
    UFUNCTION(BlueprintCallable, Category="POST|Temperature") void AddWarmZone();
    UFUNCTION(BlueprintCallable, Category="POST|Temperature") void RemoveWarmZone();
    UFUNCTION(BlueprintPure, Category="POST|Temperature") bool IsInWarmZone() const { return WarmZoneCount > 0; }
    UFUNCTION(BlueprintCallable, Category="POST|Temperature") void AddTemperature(float Amount);
    UFUNCTION(BlueprintPure, Category="POST|Temperature") float GetCurrentTemperature() const { return CurrentTemperature; }
    UFUNCTION(BlueprintPure, Category="POST|Temperature") float GetTemperaturePercent() const { return MaxTemperature > 0.0f ? CurrentTemperature / MaxTemperature : 0.0f; }
    UFUNCTION(BlueprintPure, Category="POST|Temperature") bool IsFrozen() const { return CurrentTemperature <= KINDA_SMALL_NUMBER; }

    UPROPERTY(BlueprintAssignable, Category="POST|Temperature") FPOSTBodyTemperatureChanged OnBodyTemperatureChanged;
    UPROPERTY(BlueprintAssignable, Category="POST|Temperature") FPOSTPlayerFrozen OnPlayerFrozen;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Temperature", meta=(ClampMin="1.0")) float MaxTemperature = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Temperature", meta=(ClampMin="0.0")) float CoolingPerSecond = 0.5f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Temperature", meta=(ClampMin="0.0")) float HeatingPerSecond = 1.0f;

private:
    UPROPERTY(VisibleAnywhere, Category="POST|Temperature") float CurrentTemperature = 100.0f;
    int32 WarmZoneCount = 0;
    bool bFrozenEventSent = false;
    void SetTemperature(float NewTemperature);
};
