// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponMaster.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileWeapon : public AWeaponMaster
{
	GENERATED_BODY()

public:
	virtual void fire(const FVector& hitTarget) override;
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> projectileClass;
};
