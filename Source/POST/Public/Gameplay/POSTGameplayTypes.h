#pragma once

#include "CoreMinimal.h"
#include "POSTGameplayTypes.generated.h"

UENUM(BlueprintType)
enum class EPOSTStoryStage : uint8
{
    Arrival UMETA(DisplayName="Arrival"),
    LearningTheJob UMETA(DisplayName="Learning The Job"),
    MeetingPartner UMETA(DisplayName="Meeting Partner"),
    Routine UMETA(DisplayName="Routine"),
    GrowingDoubt UMETA(DisplayName="Growing Doubt"),
    PartnerRevelation UMETA(DisplayName="Partner Revelation"),
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

UENUM(BlueprintTypes)
enum class EPOSTPresenceState : uint8
{
    Inactive,
    Warning,
    Critical
};
