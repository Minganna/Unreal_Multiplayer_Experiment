// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"


void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, defeats);
}

void ABlasterPlayerState::addToScore(float scoreAmount)
{
	SetScore(GetScore() + scoreAmount);
	character = character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : character;
	if (character)
	{
		controller = controller == nullptr ? Cast<ABlasterPlayerController>(character->Controller) : controller;
		if (controller)
		{
			controller->setHudHScore(GetScore());
		}
	}
}

void ABlasterPlayerState::addToDefeats(int32 defeatAmount)
{
	defeats += defeatAmount;
	character = character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : character;
	if (character)
	{
		controller = controller == nullptr ? Cast<ABlasterPlayerController>(character->Controller) : controller;
		if (controller)
		{
			controller->setHudDefeats(defeats);
		}
	}
}

void ABlasterPlayerState::showHideEliminatedText(bool isEliminated)
{
	character = character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : character;
	if (character)
	{
		controller = controller == nullptr ? Cast<ABlasterPlayerController>(character->Controller) : controller;
		if (controller)
		{
			controller->showHideEliminatedText(isEliminated);
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
			controller->setHudHScore(GetScore());
		}
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	character = character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : character;
	if (character)
	{
		controller = controller == nullptr ? Cast<ABlasterPlayerController>(character->Controller) : controller;
		if (controller)
		{
			controller->setHudDefeats(defeats);
		}
	}
}


