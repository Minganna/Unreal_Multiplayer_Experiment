// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void  AProjectileWeapon::fire(const FVector& hitTarget)
{
	Super::fire(hitTarget);

	if (!HasAuthority()) return;

	APawn* instigatorPawn = Cast<APawn>(GetOwner());
	// name need to be the same as the socket in the mesh of the weapon
	const USkeletalMeshSocket* muzzleFlashSocket= getWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (muzzleFlashSocket)
	{
		FTransform socketTransform = muzzleFlashSocket->GetSocketTransform(getWeaponMesh());
		// from muzzle flash socket to hit location from traceUnderCrosshairs
		FVector toTarget = hitTarget - socketTransform.GetLocation();
		FRotator targetRotation = toTarget.Rotation();
		if (projectileClass && instigatorPawn)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = GetOwner();
			spawnParams.Instigator = instigatorPawn;
			UWorld* world = GetWorld();
			if (world)
			{
				world->SpawnActor<AProjectile>(projectileClass,socketTransform.GetLocation(),targetRotation,spawnParams);
			}
		}
	}
};

