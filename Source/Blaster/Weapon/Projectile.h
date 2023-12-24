// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// the box collider for the projectile
	UPROPERTY(EditAnywhere)
	class UBoxComponent* collisionBox;
	//class that handle the movement of the projectile
	// to change bullet speed open bullet blueprint and change max and initial speed
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* projectileMovementComponent;
	// the particle system that show the bullet moving
	UPROPERTY(EditAnywhere)
	class UParticleSystem* tracer;
	// the effect spawned 
	class UParticleSystemComponent* tracerComponent;

public:	


};
