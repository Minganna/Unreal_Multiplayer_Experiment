// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Blaster/Types/TurningInPlace.h"
#include "BlasterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	//function called at the start of the game similar to Begin Play
	virtual void NativeInitializeAnimation() override;
	//called every frame
	virtual void NativeUpdateAnimation(float deltaTime) override;

private:
	//function used to initialise blaster character
	void setBlasterCharacter();
	//referece to the blaster character class
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class ABlasterCharacter* blasterCharacter;
	// speed of the main character
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float speed;
	// boolean used to check if the character is still jumping
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
	// boolean used to check if the user is adding input/ pressing any of the relevant keys
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
	// boolean used to check if the character has a weapon equipped
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsWeaponEquipped;
	// reference of the weapon equipped
	class AWeaponMaster* equippedWeapon;
	// boolean used to check if the character is crouching
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;
	// boolean used to check if the character is crouching
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;
	// variable used to drive the character strafing
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float yawOffset; 
	// variable used to drive the character leaning
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float lean;
	// variable used to determine the aim offset yaw rotation
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;
	// variable used to determine the aim offset pitch rotation
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;
	// the character rotation value of the previous frame, needed for leaning
	FRotator characterRotationLastFrame;
	// the character rotation value of the current frame, needed for leaning
	FRotator characterRotation;
	// rotator used to interpolate the yaw and avoid the clunkiness when strafing
	FRotator deltaRotation;
	// used to smoth the interpolation between two rotations
	float interpolationValue = 6.0f;
	// used to clamp a value between a maximum positive and negative value
	float clampValue = 90.0f;
	// transform that dictate the position of the left hand of the character 
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FTransform leftHandTransform;
	// enum used to detect if the camera reached a certain point and the character needs to start turning in place
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	ETurningInPlace turningInPlace;


};
