// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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

};
