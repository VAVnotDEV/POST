#include "Gameplay/POSTSleepPoint.h"
#include "Gameplay/POSTGameDirector.h"
#include "Components/SceneComponent.h"
APOSTSleepPoint::APOSTSleepPoint(){PrimaryActorTick.bCanEverTick=false;Root=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));SetRootComponent(Root);}
bool APOSTSleepPoint::CanInteract_Implementation(AActor* Interactor) const{return bCanSleep&&IsValid(Interactor);}
FText APOSTSleepPoint::GetInteractText_Implementation(AActor* Interactor) const{return FText::FromString(TEXT("Лечь спать"));}
void APOSTSleepPoint::Interact_Implementation(AActor* Interactor){if(!CanInteract_Implementation(Interactor))return;if(Director)Director->SaveProgress();OnSleepRequested(Interactor);}
