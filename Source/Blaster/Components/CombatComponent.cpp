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
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"


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

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (character)
	{
		// by default set the character walk speed to the baseWalkSpeed
		character->GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;

		if (character->getFollowCamera())
		{
			defaultFOV = character->getFollowCamera()->FieldOfView;
			currentFOV = defaultFOV;
		}
	}
	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (character && character->IsLocallyControlled())
	{
		FHitResult hitResult;
		traceUnderCrosshairs(hitResult);
		hitTarget = hitResult.ImpactPoint;

		setHUDCrosshairs(DeltaTime);
		interpFOV(DeltaTime);
	}

}

void UCombatComponent::fireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		fire();
	}
}

void UCombatComponent::fire()
{
	if (bCanFire)
	{
		serverFire(hitTarget);
		if (equippedWeapon)
		{
			// spread the crosshair when shooting
			const float shootingSpread{ 0.75f };
			crosshairShootingFactor = shootingSpread;
		}
		startFireTimer();
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

void UCombatComponent::startFireTimer()
{
	if (equippedWeapon == nullptr || character == nullptr) return;
	bCanFire = false;
	character->GetWorldTimerManager().SetTimer(fireTimer, this, &UCombatComponent::fireTimerFinished, equippedWeapon->fireDelay);
}

void UCombatComponent::fireTimerFinished()
{
	if (equippedWeapon == nullptr) return;
	bCanFire = true;
	if (bFireButtonPressed && equippedWeapon->bIsAutomatic)
	{
		fire();
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
		equippedWeapon->setWeaponState(EWeaponState::EWS_Equipped);
		// this only works if a socket has been created on the character skeleton, to do so open the skeleton, right click on the right hand bone and add socket
		// once the socket is created, rename it with a relevant name that need to be the same as the one used in this function (you might also want to rotate and reposition the socket to make the weapon look inside the character hand)
		// right click on the socket, add preview asset to simplify the step above
		const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("rightHandSocket"));
		if (handSocket)
		{
			handSocket->AttachActor(equippedWeapon, character->GetMesh());
		}
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->bUseControllerRotationYaw = true;
	}
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

void UCombatComponent::traceUnderCrosshairs(FHitResult& traceHitResult)
{
	FVector2D vieportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(vieportSize);
	}
	FVector2D crosshairLocation(vieportSize.X / 2.0f, vieportSize.Y / 2.0f);
	FVector crosshairWorldPosition;
	FVector  crosshairWorlDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), crosshairLocation, crosshairWorldPosition, crosshairWorlDirection);

	if (bScreenToWorld)
	{
		FVector start = crosshairWorldPosition;

		if (character)
		{
			float distanceToCharacter = (character->GetActorLocation() - start).Size();
			float offsetFrontOfCharacter{ 5.0f };
			start += crosshairWorlDirection * (distanceToCharacter + offsetFrontOfCharacter);
		}
		FVector end = start + crosshairWorlDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(traceHitResult, start, end, ECollisionChannel::ECC_Visibility);

		if (!traceHitResult.bBlockingHit)
		{
			traceHitResult.ImpactPoint = end;
			hitTarget = end;
		}
		else
		{
			hitTarget = traceHitResult.ImpactPoint;
		}
		// check if the hit result has the interface
		if (traceHitResult.GetActor() && traceHitResult.GetActor()->Implements<UCrosshairInteractionInterface>())
		{
			hudPackage.crosshairColor = FLinearColor::Red;
			if (isAiming)
			{

				currentAimingTargetSpread = crosshairShootingFactor + 0.10f;
			}
			else
			{
				currentAimingTargetSpread = crosshairShootingFactor + 0.50f;
			}

		}
		else
		{
			hudPackage.crosshairColor = FLinearColor::White;
			currentAimingTargetSpread = 0.0f;
		}
	}
}

void UCombatComponent::setHUDCrosshairs(float deltaTime)
{
	if (character == nullptr || character->Controller == nullptr) return;

	controller = controller == nullptr ? Cast<ABlasterPlayerController>(character->Controller) : controller;
	if (controller)
	{
		hud = hud == nullptr ? Cast<ABlasterHUD>(controller->GetHUD()) : hud;
		if (hud)
		{
			hudPackage.crosshairsCenter = nullptr;
			hudPackage.crosshairsLeft = nullptr;
			hudPackage.crosshairsRight = nullptr;
			hudPackage.crosshairsBottom = nullptr;
			hudPackage.crosshairsTop = nullptr;
			if (equippedWeapon)
			{
				hudPackage.crosshairsCenter = equippedWeapon->getCrosshairCenter();
				hudPackage.crosshairsLeft = equippedWeapon->getCrosshairLeft();
				hudPackage.crosshairsRight = equippedWeapon->getCrosshairRight();
				hudPackage.crosshairsBottom = equippedWeapon->getCrosshairBottom();
				hudPackage.crosshairsTop = equippedWeapon->getCrosshairTop();

			}

			//calculate the crosshair spread
			FVector2D walkSpeedRange(0.0f, character->GetCharacterMovement()->MaxWalkSpeed);
			if (character->bIsCrouched)
			{
				walkSpeedRange = FVector2D(0.0f, character->GetCharacterMovement()->MaxWalkSpeedCrouched);
			}

			FVector2D velocityMultiplierRange(0.0f, 1.0f);
			FVector velocity = character->GetVelocity();
			velocity.Z = 0.0f;
			//clamp player velocity to [0-1]
			crosshairVelocityFactor = FMath::GetMappedRangeValueClamped(walkSpeedRange, velocityMultiplierRange, velocity.Size());

			const float zeroValue{ 0.0f };
			const float shrinkSpreadSpeed{ 30.0f };
			if (character->GetCharacterMovement()->IsFalling())
			{
				crosshairInAirFactor = FMath::FInterpTo(crosshairInAirFactor, inAirInterp, deltaTime, inAirInterp);
			}
			else
			{
				crosshairInAirFactor = FMath::FInterpTo(crosshairInAirFactor, zeroValue, deltaTime, shrinkSpreadSpeed);
			}
			if (isAiming)
			{
				const float shrinkingValue{ 0.58f };
				crosshairAimFactor = FMath::FInterpTo(crosshairAimFactor, shrinkingValue, deltaTime, shrinkSpreadSpeed);
			}
			else
			{
				crosshairAimFactor = FMath::FInterpTo(crosshairInAirFactor, zeroValue, deltaTime, shrinkSpreadSpeed);
			}
			//reset the spread when shooting back to zero
			crosshairShootingFactor = FMath::FInterpTo(crosshairShootingFactor, zeroValue, deltaTime, shrinkSpreadSpeed);
			crosshairAimingShootableActorFactor = FMath::FInterpTo(crosshairAimingShootableActorFactor, currentAimingTargetSpread, deltaTime, shrinkSpreadSpeed);
			const float minimumSpread{ 0.5f };
			hudPackage.crosshairSpread = minimumSpread +
				crosshairVelocityFactor +
				crosshairInAirFactor -
				crosshairAimFactor -
				crosshairAimingShootableActorFactor +
				crosshairShootingFactor;
			hud->setHudPackage(hudPackage);

		}
	}
}

void UCombatComponent::interpFOV(float deltaTime)
{
	if (equippedWeapon == nullptr) return;

	if (isAiming)
	{
		currentFOV = FMath::FInterpTo(currentFOV, equippedWeapon->getZoomedFOV(), deltaTime, equippedWeapon->getZoomInterpSpeed());
	}
	else
	{
		currentFOV = FMath::FInterpTo(currentFOV, defaultFOV, deltaTime, zoomInterpSpeed);
	}
	if (character && character->getFollowCamera())
	{
		character->getFollowCamera()->SetFieldOfView(currentFOV);
	}
}
