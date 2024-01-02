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
	// Called when the gameObject is destroyed
	virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called when the projectile hit something
	UFUNCTION()
	virtual void onHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit);

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
	// the effect spawned that act as a trail
	class UParticleSystemComponent* tracerComponent;
	// particles spawned when colliding
	UParticleSystem* inpactParticles;
	// particles spawned when colliding with walls
	UPROPERTY(EditAnywhere)
	UParticleSystem* inpactParticlesWalls;
	// particles spawned when colliding with players
	UPROPERTY(EditAnywhere)
	UParticleSystem* inpactParticlesPlayers;
	// sound played when colliding
	UPROPERTY(EditAnywhere)
	class USoundCue* impactSound;

public:	


};
