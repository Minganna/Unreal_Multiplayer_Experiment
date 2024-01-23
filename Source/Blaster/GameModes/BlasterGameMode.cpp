// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

void ABlasterGameMode::playerEliminated(ABlasterCharacter* eliminaterCharacter, ABlasterPlayerController* victimController, ABlasterPlayerController* attackerController)
{
	ABlasterPlayerState* attackerPlayerState = attackerController ? Cast<ABlasterPlayerState>(attackerController->PlayerState) : nullptr;
	ABlasterPlayerState* victimPlayerState = victimController ? Cast<ABlasterPlayerState>(victimController->PlayerState) : nullptr;
	
	if (attackerPlayerState && attackerPlayerState != victimPlayerState)
	{
		attackerPlayerState->addToScore(1.0f);
	}
	if (victimPlayerState)
	{
		victimPlayerState->addToDefeats(1);
	}
	if (victimController)
	{
		victimController->showHideEliminatedText(true);
	}
	if (eliminaterCharacter != nullptr)
	{
		eliminaterCharacter->eliminated();
	}
	
}

void ABlasterGameMode::requestRespawn(ACharacter* eliminatedCharacter, AController* eliminatedController)
{
	if (eliminatedCharacter != nullptr)
	{
		eliminatedCharacter->Reset();
		eliminatedCharacter->Destroy();
	}
	if (eliminatedController != nullptr)
	{
		// save all the player starts position in the array
		TArray<AActor*> playerStarts{};
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), playerStarts);
		// ensure the spawning position are not too close to other players
		if (playerStarts.Num() > 1)
		{
			checkDistanceToPlayer(&playerStarts);
		}
		// spawn at a random position
		int32 selection = FMath::RandRange(0, playerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(eliminatedController, playerStarts[selection]);
	}
}

void ABlasterGameMode::checkDistanceToPlayer(TArray<AActor*>* spawningPoints)
{
		TArray<AActor*> players;
		UGameplayStatics::GetAllActorsOfClass(this, ACharacter::StaticClass(), players);
		const float minDistance{ 10.0f };
		TArray<AActor*> playerStartsfarFromPlayers{};
		for (auto spawningPoint : *spawningPoints)
		{
			bool bAllfarFromPlayer{ true };
			for (auto player : players)
			{
				if (calculateDistance(player->GetActorLocation(), spawningPoint->GetActorLocation()) < minDistance)
				{
					bAllfarFromPlayer = false;
				}
			}
			if (bAllfarFromPlayer)
			{
				playerStartsfarFromPlayers.Add(spawningPoint);
			}
		}
		*spawningPoints = playerStartsfarFromPlayers;
}

float ABlasterGameMode::calculateDistance(const FVector characterLocation, const FVector spawnLocation)
{
	return (characterLocation - spawnLocation).Size();
}
