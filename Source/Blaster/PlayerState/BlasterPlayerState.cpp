// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"


void ABlasterPlayerState::addToScore(float scoreAmount)
{
	Score += scoreAmount;
	character = character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : character;
	if (character)
	{
		controller = controller == nullptr ? Cast<ABlasterPlayerController>(character->Controller) : controller;
		if (controller)
		{
			controller->setHudHScore(Score);
		}
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	character = character==nullptr ? Cast<ABlasterCharacter>(GetPawn()):character;
	if (character)
	{
		controller = controller == nullptr ? Cast<ABlasterPlayerController>(character->Controller) : controller;
		if (controller)
		{
			controller->setHudHScore(Score);
		}
	}
}


