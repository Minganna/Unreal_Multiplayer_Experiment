// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//Text used to display the network role for the character, it must have the same name as the WBP_OverheadWidget text field linked to this variable
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	//function used to display/update the DisplayText value
	void setDisplayText(FString textToDisplay);
	// function used to gather the network role of a pawn character
	UFUNCTION(BlueprintCallable)
	void showPlayerNetRole(APawn* inPawn);
	//function used to get the playerName
	UFUNCTION(BlueprintCallable)
	void getPlayerName(APawn* inPawn);

protected:
	//function called when level is removed from world
	virtual void OnLevelRemovedFromWorld(ULevel* inLevel, UWorld* inWorld) override;

private:
	// float that is used as maximum of seconds to collect the player name
	UPROPERTY(EditAnywhere, Category = "Overhead Widget Properties", meta = (AllowPrivateAccess = true, Units = "seconds"))
	float getPlayerNameTimeOut = 30.0f;
	// inteval used to retry to attempt collecting the player name
	UPROPERTY(EditAnywhere, Category = "Overhead Widget Properties", meta = (AllowPrivateAccess = true, Units = "seconds"))
	float getPlayerNameInterval = 0.1f;
	// the current time used to collect the player name
	float totalTime=-0.1f;
	
};
