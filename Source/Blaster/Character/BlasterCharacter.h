// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// function used to replicate to the clients variable set on the server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// function called as soon as the components are initialised
	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Function used to move the character in forward/backward direction
	void moveForward(float value);
	//function used to move the character in side direction
	void moveRight(float value);
	//function used to turn the camera right and left
	void turn(float value);
	//function used to turn the camera up and down
	void lookUp(float value);
	// function used to detect if the equip input has been pressed
	void equipButtonPressed();

private:
	// class that allow the camera to follow the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* springArm;
	//the main camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* mainCamera;
	//Widget that display information on top of the character's head
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	// used to store a reference to the weapon the character is colliding from (that can be picked up)
	UPROPERTY(ReplicatedUsing = onRep_OverlappingWeapon)
	class AWeaponMaster* overlappingWeapon;
	// function called when overlappingWeapon get replicated
	UFUNCTION()
	void onRep_OverlappingWeapon(AWeaponMaster* lastWeapon);
	//component used to take care of all the combat logic of the player
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* combat;
	// remote procedure call function that allow a client to request the server to equip a weapon
	UFUNCTION(Server, Reliable)
	void serverEquipButtonPress();


public:	
	// setter for overlappingWeapon variable
	void setOverlappingWeapon(AWeaponMaster* weapon);


};
