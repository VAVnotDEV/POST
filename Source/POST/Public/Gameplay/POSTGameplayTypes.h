#pragma once

#include "CoreMinimal.h"
#include "POSTGameplayTypes.generated.h"

UENUM(BlueprintType)
enum class EPOSTStoryStage : uint8
{
    Arrival UMETA(DisplayName="Arrival"),
    LearningTheJob UMETA(DisplayName="Learning The Job"),
    MeetingPartner UMETA(DisplayName="Meeting Partner"),
    DrillingAnomaly UMETA(DisplayName="Drilling Anomaly"),
    Routine UMETA(DisplayName="Routine"),
    GrowingDoubt UMETA(DisplayName="Growing Doubt"),
    PartnerRevelation UMETA(DisplayName="Partner Revelation"),
    BatteryNight UMETA(DisplayName="Battery Night"),
    OpenSecondGate UMETA(DisplayName="Open Second Gate"),
    AnomaliesReleased UMETA(DisplayName="Anomalies Released"),
    FinalEscalation UMETA(DisplayName="Final Escalation"),
    Finale UMETA(DisplayName="Finale")
};

UENUM(BlueprintType)
enum class EPOSTDeathCause : uint8
{
    Unknown,
    Cold,
    Entity,
    Accident
};

UENUM(BlueprintType)
enum class EPOSTGeneratorState : uint8
{
    Stopped,
    Running,
    Broken
};
