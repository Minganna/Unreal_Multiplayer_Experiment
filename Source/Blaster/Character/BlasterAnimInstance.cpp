// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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


}

void UBlasterAnimInstance::setBlasterCharacter()
{
	if (blasterCharacter == nullptr)
	{
		blasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
}
