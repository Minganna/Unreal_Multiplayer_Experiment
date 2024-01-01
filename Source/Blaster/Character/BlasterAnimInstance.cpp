// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Weapon/WeaponMaster.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	setBlasterCharacter();
}

void UBlasterAnimInstance::NativeUpdateAnimation(float deltaTime)
{
	Super::NativeUpdateAnimation(deltaTime);
	setBlasterCharacter();
	if (blasterCharacter == nullptr)
	{
		return;
	}
	FVector velocity = blasterCharacter->GetVelocity();
	//setting velocity on z axes (vertical) to 0 as no relevant to determine velocity
	velocity.Z = 0.0f;
	speed = velocity.Size();
	// check if the character is falling
	bIsInAir = blasterCharacter->GetCharacterMovement()->IsFalling();
	// ternary operator to determine if the player is pressing a key, return true if the accelleration is more than 0
	bIsAccelerating = blasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

	bIsWeaponEquipped = blasterCharacter->isWeaponEquipped();
	equippedWeapon = blasterCharacter->getEquippedWeapon();
	bIsCrouched = blasterCharacter->bIsCrouched;
	bAiming = blasterCharacter->isAiming();
	turningInPlace = blasterCharacter->getTurningInPlace();
	bRotateRootBone = blasterCharacter->shouldRotateRootBone();

	//offset Yaw for strafing
	FRotator aimRotation = blasterCharacter->GetBaseAimRotation();
	FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(blasterCharacter->GetVelocity());
	FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation);
	// interpolate using the shorter path
	deltaRotation = FMath::RInterpTo(deltaRotation, deltaRot, deltaTime, interpolationValue);
	// getting the delta between the two world rotation to detect which way the character is strafing
	yawOffset = deltaRotation.Yaw;

	// getting the rotations to set the leaning
	characterRotationLastFrame = characterRotation;
	characterRotation = blasterCharacter->GetActorRotation();
	const FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(characterRotation, characterRotationLastFrame);
	const float target = delta.Yaw / deltaTime;
	const float interp = FMath::FInterpTo(lean, target, deltaTime, interpolationValue);
	lean = FMath::Clamp(interp, -clampValue, clampValue);

	AO_Yaw = blasterCharacter->getAO_YAW();
	AO_Pitch = blasterCharacter->getAO_Pitch();

	if (bIsWeaponEquipped && equippedWeapon && equippedWeapon->getWeaponMesh() && blasterCharacter ->GetMesh())
	{
		// create a socket in the weapon root and call it LeftHandSocket
		leftHandTransform = equippedWeapon->getWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"),ERelativeTransformSpace::RTS_World);
		//get the correct position and rotation of the left hand based on the bone of the right hand
		FVector outPosition;
		FRotator outRotator;
		blasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), leftHandTransform.GetLocation(), FRotator::ZeroRotator, outPosition, outRotator);
		leftHandTransform.SetLocation(outPosition);
		leftHandTransform.SetRotation(FQuat(outRotator));

		if (blasterCharacter->IsLocallyControlled())
		{
			bLocallyController = true;
			//ensure the weapon is pointing where the character is aiming
			FTransform handTransform = blasterCharacter->GetMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
			FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(handTransform.GetLocation(), handTransform.GetLocation() + (handTransform.GetLocation() - blasterCharacter->getHitTarget()));
			const float interpTime{ 30.0f };
			rightHandRotation = FMath::RInterpTo(rightHandRotation,lookAtRotation,deltaTime, interpTime);
		}
		
	}
}

void UBlasterAnimInstance::setBlasterCharacter()
{
	if (blasterCharacter == nullptr)
	{
		blasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
}

void UBlasterAnimInstance::playCrouchFootsteps()
{
	if (blasterCharacter && crouchFootsteps && blasterCharacter->IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), crouchFootsteps, crouchWalkVolume, 1.0f, 0.0f);
	}
}
