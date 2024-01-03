// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AProjectileBullet::onHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit)
{
	ACharacter* ownerCharacter= Cast<ACharacter>(GetOwner());
	if (ownerCharacter != nullptr)
	{
		AController* ownerController = ownerCharacter->Controller;
		if (ownerController != nullptr)
		{
			UGameplayStatics::ApplyDamage(otherActor, damage,ownerController,this,UDamageType::StaticClass());
		}
	}

	// calling super last as at the end of the function the gameobject is destroyed
	Super::onHit(hitComp,otherActor,otherComp,normalImpulse,hit);
}
