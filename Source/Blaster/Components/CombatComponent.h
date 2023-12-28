// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


#define TRACE_LENGTH 80000.0f

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
	// function called when the equippedWeapon is replicated
	UFUNCTION()
	void onRep_EquippedWeapon();
	
	// function used to determine if the fire button has been pressed or released
	void fireButtonPressed(bool bPressed);

	//server RPC call to inform the server that the character should fire
	UFUNCTION(Server, Reliable)
	void serverFire(const FVector_NetQuantize& traceHitTarget);

	// function called to all clients from the server
	UFUNCTION(NetMulticast, Reliable)
	void multicastFire(const FVector_NetQuantize& traceHitTarget);

	// fuction used to determine where the crosshair is using raytracing 
	void traceUnderCrosshairs(FHitResult& traceHitResult);
	// function used to set the crosshair texture based on the weapon the player is holding
	void setHUDCrosshairs(float deltaTime);

private: 
	// pointer to the blaster character
	class ABlasterCharacter* character;
	// pointer to the blaster player controller
	class ABlasterPlayerController* controller;
	// pointer to the blaster hud
	class ABlasterHUD* hud;
	// pointer of the currently equipped weapon
	UPROPERTY(ReplicatedUsing = onRep_EquippedWeapon)
	AWeaponMaster* equippedWeapon;
	// boolean used to check if the character is aiming
	UPROPERTY(Replicated)
	bool isAiming;
	// used to set the speed while the character is not aiming
	UPROPERTY(EditAnywhere)
	float baseWalkSpeed;
	// used to set the speed while the character is aiming
	UPROPERTY(EditAnywhere)
	float aimWalkSpeed;
	//boolean that keep track of when the fire button is pressed
	bool bFireButtonPressed;
	// position of the current target
	FVector_NetQuantize hitTarget;

	// hud and crosshairs values

	// value that is used to decide the spread of the crosshair based on character movement velocity
	float crosshairVelocityFactor{0.0f};
	// value that is used to decide the spread of the crosshair based on whenever the character is in the air 
	float crosshairInAirFactor{ 0.0f };
	// used to change the speed which the crosshair should spread when falling
	UPROPERTY(EditAnywhere)
	float inAirInterp{ 2.25f };
public:	
	

		
};
