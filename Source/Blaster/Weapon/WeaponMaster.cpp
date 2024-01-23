// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMaster.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

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
	DOREPLIFETIME(AWeaponMaster, ammo);
}

void AWeaponMaster::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		characterOwner = nullptr;
		ownerController = nullptr;
	}
	else
	{
		setHudAmmo();
	}
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
		weaponMesh->SetSimulatePhysics(false);
		weaponMesh->SetEnableGravity(false);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		weaponMesh->SetSimulatePhysics(true);
		weaponMesh->SetEnableGravity(true);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	default:
		break;
	}
}
void AWeaponMaster::setHudAmmo()
{
	characterOwner = characterOwner == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : characterOwner;
	if (characterOwner)
	{
		ownerController = ownerController == nullptr ? Cast<ABlasterPlayerController>(characterOwner->Controller) : ownerController;
		if (ownerController)
		{
			ownerController->setHudWeaponAmmo(ammo);
		}
	}
}

void AWeaponMaster::spendRound()
{
	--ammo;
	setHudAmmo();
}

void AWeaponMaster::onRep_Ammo()
{
	characterOwner = characterOwner == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : characterOwner;
	setHudAmmo();
}


void AWeaponMaster::setWeaponState(EWeaponState state)
{
	weaponState = state; 
	switch (weaponState)
	{
	case EWeaponState::EWS_Equipped:
		showPickupWidget(false);
		areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		weaponMesh->SetSimulatePhysics(false);
		weaponMesh->SetEnableGravity(false);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		weaponMesh->SetSimulatePhysics(true);
		weaponMesh->SetEnableGravity(true);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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

void AWeaponMaster::fire(const FVector& hitTarget)
{
	if (fireAnimation)
	{
		weaponMesh->PlayAnimation(fireAnimation, false);
	}
	if (casingClass)
	{
		// name need to be the same as the socket in the mesh of the weapon
		const USkeletalMeshSocket* ammoEjectSocket = weaponMesh->GetSocketByName(FName("AmmoEject"));
		if (ammoEjectSocket)
		{
			FTransform socketTransform = ammoEjectSocket->GetSocketTransform(weaponMesh);
			UWorld* world = GetWorld();
			FRotator RandomEjectRotation = FRotator(FMath::RandRange(-randRangeEject, randRangeEject), FMath::RandRange(-randRangeEject, randRangeEject), FMath::RandRange(-randRangeEject, randRangeEject));
			if (world)
			{
				world->SpawnActor<ACasing>(casingClass, socketTransform.GetLocation(), socketTransform.GetRotation().Rotator() + RandomEjectRotation);
			}
		}
	}
	spendRound();
}

void AWeaponMaster::dropped()
{
	setWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules detachRules(EDetachmentRule::KeepWorld,true);
	weaponMesh->DetachFromComponent(detachRules);
	SetOwner(nullptr);
	characterOwner = nullptr;
	ownerController = nullptr;
}

