#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTEntity.generated.h"

class APOSTCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOSTEntityStateChanged, EPOSTEntityState, OldState, EPOSTEntityState, NewState);

UCLASS(Blueprintable)
class POST_API APOSTEntity : public ACharacter
{
    GENERATED_BODY()

public:
    APOSTEntity();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintAssignable, Category="POST|Entity")
    FPOSTEntityStateChanged OnEntityStateChanged;

    UFUNCTION(BlueprintCallable, Category="POST|Entity|Perception")
    void ReportNoise(const FVector& WorldLocation, float Strength = 1.0f);

    UFUNCTION(BlueprintCallable, Category="POST|Entity|Perception")
    void ReportLight(const FVector& WorldLocation, float Strength = 1.0f);

    UFUNCTION(BlueprintPure, Category="POST|Entity")
    float GetAwareness() const { return Awareness; }

    UFUNCTION(BlueprintPure, Category="POST|Entity")
    EPOSTEntityState GetEntityState() const { return EntityState; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Perception", meta=(ClampMin="0.0"))
    float PerceptionRadius = 2500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Perception", meta=(ClampMin="0.0"))
    float MovementAwarenessPerSecond = 22.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Perception", meta=(ClampMin="0.0"))
    float RunningMultiplier = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Perception", meta=(ClampMin="0.0"))
    float AwarenessDecayPerSecond = 9.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Perception", meta=(ClampMin="0.0"))
    float NightAwarenessMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Decision", meta=(ClampMin="0.0", ClampMax="100.0"))
    float InterestedThreshold = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Decision", meta=(ClampMin="0.0", ClampMax="100.0"))
    float HuntingThreshold = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Decision", meta=(ClampMin="0.0"))
    float AttackDistance = 140.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Decision", meta=(ClampMin="0.0"))
    float SearchAcceptanceRadius = 100.0f;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Entity")
    void OnAttackPlayer(APOSTCharacter* Player);

private:
    void UpdatePerception(float DeltaTime);
    void UpdateDecision();
    void SetEntityState(EPOSTEntityState NewState);
    void MoveToward(const FVector& Location);
    void StopEntityMovement();
    void TryAttack();
    float GetTimeOfDayMultiplier() const;
    float GetDistanceMultiplier(float Distance) const;

    UPROPERTY(Transient)
    APOSTCharacter* Player = nullptr;

    UPROPERTY(VisibleAnywhere, Category="POST|Entity")
    float Awareness = 0.0f;

    UPROPERTY(VisibleAnywhere, Category="POST|Entity")
    EPOSTEntityState EntityState = EPOSTEntityState::Roaming;

    FVector LastKnownPlayerLocation = FVector::ZeroVector;
    bool bHasLastKnownLocation = false;
    bool bAttackCommitted = false;
};
