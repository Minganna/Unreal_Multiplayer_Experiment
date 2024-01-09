// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
public: 
	//function called when the score is updated
	virtual void OnRep_Score() override;
	void addToScore(float scoreAmount);
private:
	// reference to the main character
	class ABlasterCharacter* character;
	// reference to the main player controller
	class ABlasterPlayerController* controller;
};
