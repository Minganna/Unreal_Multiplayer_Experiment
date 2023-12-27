// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ACasing::ACasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	casingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(casingMesh);
	casingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	casingMesh->SetSimulatePhysics(true);
	casingMesh->SetEnableGravity(true);
	casingMesh->SetNotifyRigidBodyCollision(true);
	shellEjectionImpulse = 10.0f;

}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	casingMesh->OnComponentHit.AddDynamic(this,&ACasing::onHit);
	casingMesh->AddImpulse(GetActorForwardVector()*shellEjectionImpulse);
}

void ACasing::onHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit)
{
	if (shellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, shellSound, GetActorLocation());
	}
	GetWorldTimerManager().SetTimer(destroyShellHandle, this, &ACasing::destroyDelay, delayTime, true, delayTime);
}

void ACasing::destroyDelay()
{
	GetWorldTimerManager().ClearTimer(destroyShellHandle);
	Destroy();
}




