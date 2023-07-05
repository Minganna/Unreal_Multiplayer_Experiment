// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeaponMaster;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// allow full access to character class
	friend class ABlasterCharacter;
	// function used to equip a weapon to the character
	void equipWeapon(AWeaponMaster* weaponToequip);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private: 
	// pointer to the blaster character
	class ABlasterCharacter* character;
	// pointer of the currently equipped weapon
	AWeaponMaster* equippedWeapon;
public:	
	

		
};
