#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "POSTHUD.generated.h"

UCLASS()
class POST_API APOSTHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

protected:
    UPROPERTY(EditDefaultsOnly, Category="POST|HUD")
    float BarWidth = 220.0f;

    UPROPERTY(EditDefaultsOnly, Category="POST|HUD")
    float BarHeight = 16.0f;

    UPROPERTY(EditDefaultsOnly, Category="POST|HUD")
    float ScreenPadding = 30.0f;

    UPROPERTY(EditDefaultsOnly, Category="POST|HUD")
    FLinearColor BackgroundColor = FLinearColor(0.02f, 0.02f, 0.02f, 0.75f);

    UPROPERTY(EditDefaultsOnly, Category="POST|HUD")
    FLinearColor StaminaColor = FLinearColor(0.75f, 0.75f, 0.75f, 0.9f);

    UPROPERTY(EditDefaultsOnly, Category="POST|HUD")
    FLinearColor TemperatureColor = FLinearColor(0.4f, 0.7f, 1.0f, 0.9f);

private:
    void DrawStatusBar(const FString& Label, float Value, float Percent, float X, float Y, const FLinearColor& FillColor);
};
