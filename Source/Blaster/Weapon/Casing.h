// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACasing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called when the casing hit something
	UFUNCTION()
	virtual void onHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit);

private:
	//the bullet shell mesh 
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* casingMesh;
	// the force that is used to eject the shell
	UPROPERTY(EditAnywhere)
	float shellEjectionImpulse;
	// sound played when shell hit the ground
	UPROPERTY(EditAnywhere)
	class USoundCue* shellSound;
	//the timer handle
	struct FTimerHandle destroyShellHandle;
	//delay the destruction of the shell using a timer
	void destroyDelay();
	//the callback time for the timer
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	float delayTime = 0.3f;

};
