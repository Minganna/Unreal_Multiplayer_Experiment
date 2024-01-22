// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	// variable binded to the widget visible property for the health bar
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* healthBar;
	// variable binded to the widget visible property for the health text
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* healthText;
	// variable binded to the widget visible property for the eliminated text
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* eliminatedText;
	// variable binded to the widget visible property for the Score text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* scoreAmount;
	// variable binded to the widget visible property for the Defeats text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* defeatAmount;

};
