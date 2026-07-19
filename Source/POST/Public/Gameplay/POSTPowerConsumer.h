#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTPowerConsumer.generated.h"

class APOSTGenerator;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTPowerChanged, bool, bHasPower);

UCLASS(Blueprintable)
class POST_API APOSTPowerConsumer : public AActor
{
    GENERATED_BODY()
public:
    APOSTPowerConsumer();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintPure, Category="POST|Power") bool IsPowered() const { return bPowered; }
    UFUNCTION(BlueprintPure, Category="POST|Power") bool IsEnabled() const { return bEnabled; }
    UFUNCTION(BlueprintCallable, Category="POST|Power") void SetEnabled(bool bNewEnabled);
    UFUNCTION(BlueprintCallable, Category="POST|Power") void SetGenerator(APOSTGenerator* NewGenerator);

    UPROPERTY(BlueprintAssignable, Category="POST|Power") FPOSTPowerChanged OnPowerStateChanged;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="POST|Power") APOSTGenerator* Generator = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Power") bool bAutoFindGenerator = true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Power") bool bEnabled = true;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Power") bool bPowered = false;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Power") void OnPowerChanged(bool bHasPower);

private:
    UFUNCTION() void HandleGeneratorStateChanged(EPOSTGeneratorState NewState);
    UFUNCTION() void HandleGeneratorFuelChanged(float NewFuel);
    void BindGenerator();
    void UnbindGenerator();
    void RefreshPower();
};
