#include "UI/POSTHUD.h"

#include "Components/POSTInteractionComponent.h"
#include "Components/POSTStaminaComponent.h"
#include "Components/POSTTemperatureComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Player/POSTCharacter.h"

void APOSTHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas || !PlayerOwner)
    {
        return;
    }

    const APOSTCharacter* Character = Cast<APOSTCharacter>(PlayerOwner->GetPawn());
    if (!Character)
    {
        return;
    }

    const UPOSTStaminaComponent* Stamina = Character->GetStaminaComponent();
    const UPOSTTemperatureComponent* Temperature = Character->GetTemperatureComponent();
    const UPOSTInteractionComponent* Interaction = Character->GetInteractionComponent();

    float Y = Canvas->ClipY - ScreenPadding - BarHeight;
    if (Stamina)
    {
        DrawStatusBar(TEXT("Выносливость"), Stamina->GetCurrentStamina(), Stamina->GetStaminaPercent(), ScreenPadding, Y, StaminaColor);
        Y -= BarHeight + 24.0f;
    }

    if (Temperature)
    {
        DrawStatusBar(TEXT("Температура"), Temperature->GetCurrentTemperature(), Temperature->GetTemperaturePercent(), ScreenPadding, Y, TemperatureColor);
    }

    if (Interaction)
    {
        const FText InteractionText = Interaction->GetCurrentInteractText();
        if (!InteractionText.IsEmpty())
        {
            const FString Prompt = FString::Printf(TEXT("[E] %s"), *InteractionText.ToString());
            float TextWidth = 0.0f;
            float TextHeight = 0.0f;
            GetTextSize(Prompt, TextWidth, TextHeight, GEngine->GetMediumFont(), 1.0f);

            const float X = (Canvas->ClipX - TextWidth) * 0.5f;
            const float PromptY = Canvas->ClipY * 0.72f;
            DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.65f), X - 12.0f, PromptY - 7.0f, TextWidth + 24.0f, TextHeight + 14.0f);
            DrawText(Prompt, FLinearColor::White, X, PromptY, GEngine->GetMediumFont(), 1.0f, false);
        }
    }
}

void APOSTHUD::DrawStatusBar(const FString& Label, float Value, float Percent, float X, float Y, const FLinearColor& FillColor)
{
    const float ClampedPercent = FMath::Clamp(Percent, 0.0f, 1.0f);
    DrawRect(BackgroundColor, X, Y, BarWidth, BarHeight);
    DrawRect(FillColor, X + 2.0f, Y + 2.0f, (BarWidth - 4.0f) * ClampedPercent, BarHeight - 4.0f);

    const FString Text = FString::Printf(TEXT("%s: %.0f"), *Label, Value);
    DrawText(Text, FLinearColor::White, X, Y - 19.0f, GEngine->GetSmallFont(), 1.0f, false);
}
