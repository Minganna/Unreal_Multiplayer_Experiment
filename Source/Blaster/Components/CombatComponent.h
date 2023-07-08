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
	// function used to replicate to the clients variable set on the server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// allow full access to character class
	friend class ABlasterCharacter;
	// function used to equip a weapon to the character
	void equipWeapon(AWeaponMaster* weaponToequip);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// function used to ensure that server and client can see character aiming
	void setAiming(bool bAiming);
	// remote procedure call function that allow a client to request the server to aim
	UFUNCTION(Server, Reliable)
	void serverSetAiming(bool bAiming);

private: 
	// pointer to the blaster character
	class ABlasterCharacter* character;
	// pointer of the currently equipped weapon
	UPROPERTY(Replicated)
	AWeaponMaster* equippedWeapon;
	// boolean used to check if the character is aiming
	UPROPERTY(Replicated)
	bool isAiming;

public:	
	

		
};
