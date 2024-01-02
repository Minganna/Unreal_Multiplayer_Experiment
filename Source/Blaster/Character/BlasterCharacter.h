// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/Types/TurningInPlace.h"
#include "Blaster/Interfaces/CrosshairInteractionInterface.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public ICrosshairInteractionInterface
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

	//play animation montages
	// 
	// function used to play the fire anim montage
	void playFireMontage(bool bAiming);
	// ensure the hit reaction animation montage is played on each machines
	UFUNCTION(NetMulticast,Unreliable)
	void multicastHit();
	// used to ensure simulated proxy can see the character turn correctly
	virtual void OnRep_ReplicatedMovement() override;


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
	// function used to detect if the crouch input has been pressed
	void crouchButtonPressed();
	// function used to detect if the crouch input has been released
	void crouchButtonReleased();
	// function used to detect if the aim input has been pressed
	void aimButtonPressed();
	// function used to detect if the aim input has been released
	void aimButtonReleased();
	// function used to calculate the yaw and pitch offset
	void aimOffset(float deltaTime);
	// function used to calculate the pitch of the top part of the character body
	void calculateAO_Pitch();
	// function used to ensure that the simulated proxy can see the character turn without gittering
	void simProxyTurn();
	//Add possibility to override the Jump function
	virtual void Jump() override;
	// function called when the fire button is pressed and the character should fire the weapon
	void fireButtonPressed();
	// function used when the fire button is released
	void fireButtonReleased();

	//play animation montages
	// function used to play the animation montage used when the character gets hit
	void playHitReactMontage();

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
	// variable used to determine the aim offset yaw rotation
	float AO_Yaw;
	// value used to check if the character needs to rotate the root bone
	float interpAO_Yaw;
	// variable used to determine the aim offset pitch rotation
	float AO_Pitch;
	// used to keep track of the rotation when not moving
	FRotator startingAimRotation;
	// enum used to detect if the camera reached a certain point and the character needs to start turning in place
	ETurningInPlace turningInPlace;
	// function used to determine if the character should turn in place base by the offset yaw
	void turnInPlace(float deltaTime);
	// function used to calculate the character speed
	float calculateSpeed();

	//function used to not render the character if it is too close to the camera
	void hideCameraIfCharacterClose();
	// the threshold used to define if the character is too close
	UPROPERTY(EditAnywhere)
	float cameraThreshold{ 200.0f };


	//animation montages

	//pointer to the animation montage used to fire the weapon
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* fireWeaponMontage;
	//pointer to the animation montage used when the character gets hit
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* hitReactMontage;

	//simulated proxy logic

	// rotators and float used to calculate how much the proxy character rotated in the last frame
	FRotator proxyRotationLastFrame{};
	FRotator proxyRotation{};
	float proxyYaw{};
	//float used to keep track when was the last time that OnRep_ReplicatedMovement() was called
	float timeSinceLastMovementReplication{ 0.0f };
	//bool that is used to determine whenever or not the root bone should be rotated in simulated proxy
	bool bRotateRootBone{ false };
	// used to determine in simulated proxy when the character should turn the root bone
	float turnTreshold{ 0.5f };

	//player health
	//the maximum value for the player health 
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float maxHealth{100.0f};
	// the current value for player health
	UPROPERTY(ReplicatedUsing= onRep_Health, VisibleAnywhere, Category = "Player Stats")
	float health{ maxHealth };
	// rep notifier called when health value change
	UFUNCTION()
	void onRep_Health();

public:	
	// setter for overlappingWeapon variable
	void setOverlappingWeapon(AWeaponMaster* weapon);
	// getter for the weapon equipped variable
	bool isWeaponEquipped();
	//getter for the aiming bool
	bool isAiming();
	//getter for the aim offset yaw
	FORCEINLINE float getAO_YAW() const { return AO_Yaw; }
	//getter for the aim offset pitch
	FORCEINLINE float getAO_Pitch() const { return AO_Pitch; }
	//getter for turning in place
	FORCEINLINE ETurningInPlace getTurningInPlace() const { return turningInPlace; }
	//getter of the equipped weapon
	AWeaponMaster* getEquippedWeapon();
	//getter for the hitTarget
	FVector getHitTarget() const;
	//getter for the camera component
	FORCEINLINE UCameraComponent* getFollowCamera() const { return mainCamera; }
	//getter for bRotateRootBone
	FORCEINLINE bool shouldRotateRootBone() const { return bRotateRootBone; }
};
