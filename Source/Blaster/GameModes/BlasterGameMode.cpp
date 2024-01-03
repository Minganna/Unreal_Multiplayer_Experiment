// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

void ABlasterGameMode::playerEliminated(ABlasterCharacter* eliminaterCharacter, ABlasterPlayerController* victimController, ABlasterPlayerController* attackerController)
{
	if (eliminaterCharacter != nullptr)
	{
		eliminaterCharacter->eliminated();
	}
	
}
