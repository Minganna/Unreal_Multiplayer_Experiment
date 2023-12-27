// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Blaster/Weapon/WeaponMaster.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	baseWalkSpeed = 600.0f;
	aimWalkSpeed = 450.0f;

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, equippedWeapon);
	DOREPLIFETIME(UCombatComponent, isAiming);
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

void UCombatComponent::fireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		FHitResult hitResult;
		traceUnderCrosshairs(hitResult);
		serverFire(hitTarget);
	}
}

void UCombatComponent::traceUnderCrosshairs(FHitResult& traceHitResult)
{
	FVector2D vieportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(vieportSize);
	}
	FVector2D crosshairLocation(vieportSize.X/ 2.0f, vieportSize.Y/ 2.0f);
	FVector crosshairWorldPosition;
	FVector  crosshairWorlDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), crosshairLocation, crosshairWorldPosition, crosshairWorlDirection);

	if (bScreenToWorld)
	{
		FVector start = crosshairWorldPosition;

		FVector end = start + crosshairWorlDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(traceHitResult,start,end,ECollisionChannel::ECC_Visibility);

		if (!traceHitResult.bBlockingHit)
		{
			traceHitResult.ImpactPoint = end;
			hitTarget = end;
		}
		else
		{
			hitTarget = traceHitResult.ImpactPoint;
		}
	}
}

void UCombatComponent::serverFire_Implementation(const FVector_NetQuantize& traceHitTarget)
{
	multicastFire(traceHitTarget);
}

void UCombatComponent::multicastFire_Implementation(const FVector_NetQuantize& traceHitTarget)
{
	if (equippedWeapon == nullptr) return;

	if (character)
	{
		character->playFireMontage(isAiming);
		equippedWeapon->fire(traceHitTarget);
	}
}




// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// debug code to be removed

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

