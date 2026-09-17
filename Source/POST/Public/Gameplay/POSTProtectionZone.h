#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTProtectionZone.generated.h"

class UBoxComponent;
class UNavModifierComponent;

UCLASS()
class POST_API APOSTProtectionZone : public AActor
{
    GENERATED_BODY()

public:
    APOSTProtectionZone();

    UFUNCTION(BlueprintPure, Category="POST|Protection")
    bool IsActorInside(const AActor* Actor) const;

    UFUNCTION(BlueprintPure, Category="POST|Protection", meta=(WorldContext="WorldContextObject"))
    static bool IsActorProtected(const UObject* WorldContextObject, const AActor* Actor);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Protection")
    UBoxComponent* ProtectionBox;

    // Marks the whole protected volume as NavArea_Null. AI cannot build a path
    // through the checkpoint, while the player can still walk through normally.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POST|Protection")
    UNavModifierComponent* NavigationBlocker;
};
