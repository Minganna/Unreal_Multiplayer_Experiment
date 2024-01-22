// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
public: 
	//function called when one of the player dies
	virtual void playerEliminated(class ABlasterCharacter* eliminaterCharacter, class ABlasterPlayerController* victimController, ABlasterPlayerController* attackerController);
	virtual void requestRespawn(class ACharacter* eliminatedCharacter, AController* eliminatedController);
	void checkDistanceToPlayer(TArray<AActor*>* spawningPoints);
	float calculateDistance(const FVector characterLocation,const FVector spawnLocation);
};
