// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// function used to update the value of health in the hud
	void setHudHealth(const float health, const float maxHealth);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	// pointer to the main game hud
	class ABlasterHUD* blasterHud;
};
