#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTPowerConsumer.generated.h"
class APOSTGenerator;

UCLASS(Blueprintable)
class POST_API APOSTPowerConsumer : public AActor
{
    GENERATED_BODY()
public:
    APOSTPowerConsumer();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    UFUNCTION(BlueprintPure, Category="POST|Power") bool IsPowered() const { return bPowered; }
    UFUNCTION(BlueprintCallable, Category="POST|Power") void SetEnabled(bool bNewEnabled);
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="POST|Power") APOSTGenerator* Generator;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Power") bool bEnabled = true;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Power") bool bPowered = false;
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Power") void OnPowerChanged(bool bHasPower);
private:
    void RefreshPower();
};
