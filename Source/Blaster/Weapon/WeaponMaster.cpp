// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMaster.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponMaster::AWeaponMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	SetRootComponent(weaponMesh);

	// as the gun can be dropped, we don't want it to collide with the player
	weaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	areaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	areaSphere->SetupAttachment(RootComponent);
	// activate collision on the server as this is a multiplayer game
	areaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	pickupWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponMaster::BeginPlay()
{
	Super::BeginPlay();

	if (pickupWidget)
	{
		pickupWidget->SetVisibility(false);
	}

	// if server enable collision
	if (HasAuthority())
	{
		areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		areaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		areaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponMaster::OnSphereOverlap);
		areaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponMaster::OnSphereEndOverlap);
	}
	
}

// Called every frame
void AWeaponMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponMaster, weaponState);
}

void AWeaponMaster::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	// check if object overlapping is player, if so show message
	ABlasterCharacter* blasterCharacter = Cast<ABlasterCharacter>(otherActor);
	if (blasterCharacter)
	{
		
		blasterCharacter->setOverlappingWeapon(this);
	}
}

void AWeaponMaster::OnSphereEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	// check if object overlapping is player, if so show message
	ABlasterCharacter* blasterCharacter = Cast<ABlasterCharacter>(otherActor);
	if (blasterCharacter)
	{

		blasterCharacter->setOverlappingWeapon(nullptr);
	}
}

void AWeaponMaster::onRep_WeaponState()
{
	switch (weaponState)
	{
	case EWeaponState::EWS_Equipped:
		showPickupWidget(false);
		break;
	default:
		break;
	}
}

void AWeaponMaster::setWeaponState(EWeaponState state)
{
	weaponState = state; 
	switch (weaponState)
	{
	case EWeaponState::EWS_Equipped:
		showPickupWidget(false);
		areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	default:
		break;
	}
}



void AWeaponMaster::showPickupWidget(bool bShowWidget)
{
	if (pickupWidget)
	{
		pickupWidget->SetVisibility(bShowWidget);
	}
}

