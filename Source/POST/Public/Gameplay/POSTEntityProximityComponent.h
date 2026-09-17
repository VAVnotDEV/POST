#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POSTEntityProximityComponent.generated.h"

class APOSTEntity;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOSTEntityProximityChanged, float, Intensity);

UCLASS(ClassGroup=(POST), meta=(BlueprintSpawnableComponent))
class POST_API UPOSTEntityProximityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPOSTEntityProximityComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(BlueprintAssignable, Category="POST|Entity Proximity")
    FPOSTEntityProximityChanged OnProximityChanged;

    UFUNCTION(BlueprintPure, Category="POST|Entity Proximity")
    float GetProximityIntensity() const { return ProximityIntensity; }

    UFUNCTION(BlueprintPure, Category="POST|Entity Proximity")
    float GetEntityDistance() const { return EntityDistance; }

protected:
    // No interference outside this distance.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity Proximity", meta=(ClampMin="0.0"))
    float MaxSignalDistance = 3000.0f;

    // At and below this distance the signal is fully saturated.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity Proximity", meta=(ClampMin="0.0"))
    float FullSignalDistance = 350.0f;

    // Smooths the radio/static response so it does not jump every frame.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity Proximity", meta=(ClampMin="0.0"))
    float InterpSpeed = 4.0f;

private:
    UPROPERTY(Transient)
    APOSTEntity* Entity = nullptr;

    UPROPERTY(VisibleAnywhere, Category="POST|Entity Proximity")
    float ProximityIntensity = 0.0f;

    UPROPERTY(VisibleAnywhere, Category="POST|Entity Proximity")
    float EntityDistance = 0.0f;
};
