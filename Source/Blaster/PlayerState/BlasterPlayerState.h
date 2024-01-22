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
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
	//replication notifies

	//function called when the score is updated
	virtual void OnRep_Score() override;
	//function called when the defeats is updated
	UFUNCTION()
	virtual void OnRep_Defeats();
	// function used to update the score value
	void addToScore(float scoreAmount);
	// function used to update the defeat value
	void addToDefeats(int32 defeatAmount);
	// function used to show or hide the eliminated text
	void showHideEliminatedText(bool isEliminated);
private:
	// reference to the main character
	class ABlasterCharacter* character{nullptr};
	// reference to the main player controller
	class ABlasterPlayerController* controller{ nullptr };
	// integer that store the number of times the character died
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 defeats{};
};
