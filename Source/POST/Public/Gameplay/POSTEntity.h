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

    UFUNCTION(BlueprintPure, Category="POST|Entity|Debug")
    bool HasLastKnownPlayerLocation() const { return bHasLastKnownLocation; }

    UFUNCTION(BlueprintPure, Category="POST|Entity|Debug")
    FVector GetLastKnownPlayerLocation() const { return LastKnownPlayerLocation; }

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Search", meta=(ClampMin="0.0"))
    float SearchDuration = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Search", meta=(ClampMin="0.0"))
    float SearchRadius = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Search", meta=(ClampMin="0.1"))
    float SearchMoveInterval = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Roaming", meta=(ClampMin="0.0"))
    float RoamingRadius = 2200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Roaming", meta=(ClampMin="0.1"))
    float RoamingMoveInterval = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Entity|Roaming", meta=(ClampMin="0.0"))
    float RoamingAcceptanceRadius = 120.0f;

    UFUNCTION(BlueprintImplementableEvent, Category="POST|Entity")
    void OnAttackPlayer(APOSTCharacter* VictimCharacter);

private:
    void UpdatePerception(float DeltaTime);
    void UpdateDecision(float DeltaTime);
    void SetEntityState(EPOSTEntityState NewState);
    void MoveToward(const FVector& Location, float AcceptanceRadius = -1.0f);
    void StopEntityMovement();
    void TryAttack();
    void RememberPlayerLocation(const FVector& Location);
    void BeginSearch();
    void UpdateSearch(float DeltaTime);
    void UpdateRoaming(float DeltaTime);
    void ForgetPlayer();
    float GetTimeOfDayMultiplier() const;
    float GetDistanceMultiplier(float Distance) const;

    UPROPERTY(Transient)
    APOSTCharacter* TargetPlayer = nullptr;

    UPROPERTY(VisibleAnywhere, Category="POST|Entity")
    float Awareness = 0.0f;

    UPROPERTY(VisibleAnywhere, Category="POST|Entity")
    EPOSTEntityState EntityState = EPOSTEntityState::Roaming;

    FVector SpawnLocation = FVector::ZeroVector;
    FVector LastKnownPlayerLocation = FVector::ZeroVector;
    bool bHasLastKnownLocation = false;
    bool bReceivedPlayerStimulusThisFrame = false;
    bool bAttackCommitted = false;
    bool bSearchAreaReached = false;
    float SearchTimeRemaining = 0.0f;
    float SearchMoveCooldown = 0.0f;
    float RoamingMoveCooldown = 0.0f;
};
