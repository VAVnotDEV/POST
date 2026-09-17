#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSTProtectionZone.generated.h"

class UBoxComponent;

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
};
