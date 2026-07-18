#include "Gameplay/POSTBarrier.h"
#include "Components/SceneComponent.h"
APOSTBarrier::APOSTBarrier(){ PrimaryActorTick.bCanEverTick=true; Root=CreateDefaultSubobject<USceneComponent>(TEXT("Root")); SetRootComponent(Root); MovablePart=CreateDefaultSubobject<USceneComponent>(TEXT("MovablePart")); MovablePart->SetupAttachment(Root); }
void APOSTBarrier::Tick(float DeltaSeconds){ Super::Tick(DeltaSeconds); const FRotator Target=bIsOpen?OpenRotation:ClosedRotation; MovablePart->SetRelativeRotation(FMath::RInterpTo(MovablePart->GetRelativeRotation(),Target,DeltaSeconds,MoveSpeed)); }
bool APOSTBarrier::CanInteract_Implementation(AActor* Interactor) const{return IsValid(Interactor);}
FText APOSTBarrier::GetInteractText_Implementation(AActor* Interactor) const{return FText::FromString(bIsOpen?TEXT("Закрыть шлагбаум"):TEXT("Открыть шлагбаум"));}
void APOSTBarrier::Interact_Implementation(AActor* Interactor){SetOpen(!bIsOpen);}
void APOSTBarrier::SetOpen(bool bOpen){bIsOpen=bOpen;}
