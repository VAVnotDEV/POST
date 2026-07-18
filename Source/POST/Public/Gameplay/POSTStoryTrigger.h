#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Gameplay/POSTGameplayTypes.h"
#include "POSTStoryTrigger.generated.h"
class APOSTGameDirector;
UCLASS(Blueprintable)
class POST_API APOSTStoryTrigger : public AActor
{
    GENERATED_BODY()
public:
    APOSTStoryTrigger();
protected:
    virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent* Trigger;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="POST|Story") APOSTGameDirector* Director;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Story") EPOSTStoryStage RequiredStage = EPOSTStoryStage::Arrival;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Story") EPOSTStoryStage NextStage = EPOSTStoryStage::LearningTheJob;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POST|Story") bool bDisableAfterTrigger = true;
    UFUNCTION() void HandleOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
    UFUNCTION(BlueprintImplementableEvent, Category="POST|Story") void OnStoryTriggered(AActor* TriggeringActor);
};
