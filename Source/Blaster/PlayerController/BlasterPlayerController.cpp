// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"



void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	blasterHud = Cast<ABlasterHUD>(GetHUD());

}
void ABlasterPlayerController::OnPossess(APawn* inPawn)
{
	Super::OnPossess(inPawn);
	ABlasterCharacter* blasterCharacter = Cast<ABlasterCharacter>(inPawn);
	setHudHealth(blasterCharacter->getHealth(), blasterCharacter->getMaxHealth());
	// ensure that the text is hidden when the player can return to play the game
	showHideEliminatedText(false);
}

void ABlasterPlayerController::setHudHealth(const float health,const float maxHealth)
{
	blasterHud = blasterHud == nullptr ? Cast<ABlasterHUD>(GetHUD()) : blasterHud;
	bool bHudValid = blasterHud && blasterHud->characterOverlay && blasterHud->characterOverlay->healthBar && blasterHud->characterOverlay->healthText;
	if (bHudValid)
	{
		const float healthPercent{health/maxHealth};
		blasterHud->characterOverlay->healthBar->SetPercent(healthPercent);
		FString healthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(health), FMath::CeilToInt(maxHealth));
		blasterHud->characterOverlay->healthText->SetText(FText::FromString(healthText));
	}
}

void ABlasterPlayerController::setHudHScore(const float score)
{
	blasterHud = blasterHud == nullptr ? Cast<ABlasterHUD>(GetHUD()) : blasterHud;
	bool bHudValid = blasterHud && blasterHud->characterOverlay && blasterHud->characterOverlay->scoreAmount;
	if (bHudValid)
	{
		FString scoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(score));
		blasterHud->characterOverlay->scoreAmount->SetText(FText::FromString(scoreText));
	}

}

void ABlasterPlayerController::setHudDefeats(const int32 defeats)
{
	blasterHud = blasterHud == nullptr ? Cast<ABlasterHUD>(GetHUD()) : blasterHud;
	bool bHudValid = blasterHud && blasterHud->characterOverlay && blasterHud->characterOverlay->defeatAmount;
	if (bHudValid)
	{
		FString defeatText = FString::Printf(TEXT("%d"), defeats);
		blasterHud->characterOverlay->defeatAmount->SetText(FText::FromString(defeatText));
	}
}

void ABlasterPlayerController::showHideEliminatedText(bool isActive)
{
	blasterHud = blasterHud == nullptr ? Cast<ABlasterHUD>(GetHUD()) : blasterHud;
	bool bHudValid = blasterHud && blasterHud->characterOverlay && blasterHud->characterOverlay->eliminatedText;
	if (bHudValid)
	{
		if (isActive)
		{
			blasterHud->characterOverlay->eliminatedText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			blasterHud->characterOverlay->eliminatedText->SetVisibility(ESlateVisibility::Hidden);
		}
		
	}
}




