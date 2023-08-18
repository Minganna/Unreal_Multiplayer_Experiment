// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Blaster/Weapon/WeaponMaster.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	baseWalkSpeed = 600.0f;
	aimWalkSpeed = 450.0f;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (character)
	{
		// by default set the character walk speed to the baseWalkSpeed
		character->GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
	}
	
}

void UCombatComponent::setAiming(bool bAiming)
{
	isAiming = bAiming;
	serverSetAiming(isAiming);
	// set the speed on the client
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = isAiming ? aimWalkSpeed : baseWalkSpeed;
	}
}

FVector UCombatComponent::grapplingLineTrace(FVector currentPos)
{
	FVector locationToGrapple = currentPos;
	isGrappling = false;
	if (character)
	{
		UCameraComponent* mainCamera = character->getMainCamera();

		if (mainCamera)
		{
			FVector characterLocation = currentPos;
			FVector cameraFwd = mainCamera->GetForwardVector();
			FVector cameraFwdLength = (cameraFwd * 40000.0f) + characterLocation;
			FVector forwardMinusCollider = cameraFwd * (-34.0f);
			


			FHitResult outHit;
			FCollisionQueryParams traceParams;
			traceParams.AddIgnoredActor(character);
			GetWorld()->LineTraceSingleByChannel(outHit, characterLocation, cameraFwdLength, ECC_Pawn, traceParams);
			//DrawDebugLine(GetWorld(), characterLocation, cameraFwdLength, outHit.bBlockingHit ? FColor::Blue : FColor::Red, false, 2.0f);
			if (outHit.bBlockingHit)
			{
				isGrappling = true;
				locationToGrapple = outHit.ImpactPoint + forwardMinusCollider;
			}
		}
	}
	return locationToGrapple;
}

void UCombatComponent::serverSetAiming_Implementation(bool bAiming)
{
	isAiming = bAiming;
	// set the speed on the server to ensure that each client see the correct speed
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = isAiming ? aimWalkSpeed : baseWalkSpeed;
	}
}


void UCombatComponent::onRep_EquippedWeapon()
{
	if (equippedWeapon && character)
	{
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->bUseControllerRotationYaw = true;
	}
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, equippedWeapon);
	DOREPLIFETIME(UCombatComponent, isAiming);
}

void UCombatComponent::equipWeapon(AWeaponMaster* weaponToequip)
{
	if (character == nullptr || weaponToequip == nullptr) return;

	equippedWeapon = weaponToequip;
	equippedWeapon->setWeaponState(EWeaponState::EWS_Equipped);
	// this only works if a socket has been created on the character skeleton, to do so open the skeleton, right click on the right hand bone and add socket
	// once the socket is created, rename it with a relevant name that need to be the same as the one used in this function (you might also want to rotate and reposition the socket to make the weapon look inside the character hand)
	// right click on the socket, add preview asset to simplify the step above
	const USkeletalMeshSocket* handSocket= character->GetMesh()->GetSocketByName(FName("rightHandSocket"));
	if (handSocket)
	{
		handSocket->AttachActor(equippedWeapon,character->GetMesh());
	}
	equippedWeapon->SetOwner(character);
	character->GetCharacterMovement()->bOrientRotationToMovement = false;
	character->bUseControllerRotationYaw = true;
}

