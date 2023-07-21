// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/WeaponMaster.h"
#include "Blaster/Components/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create the spring arm and attach it to the capsule collider
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	springArm->SetupAttachment(GetMesh());
	springArm->TargetArmLength = 600.0f;
	springArm->bUsePawnControlRotation = true;
	//Create the Camera and attach it to the spring arm
	mainCamera = CreateDefaultSubobject <UCameraComponent>(TEXT("Main Camera"));
	mainCamera->SetupAttachment(springArm, USpringArmComponent::SocketName);
	mainCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
	
	combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	combat->SetIsReplicated(true);
	// in constructor ensure that the character can crouch by setting the boolean
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	// avoid the collision with the camera
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, overlappingWeapon, COND_OwnerOnly);
}


// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	aimOffset(DeltaTime);

}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind the action (button pressed) from Edit/ProjectSettings/Input to a specific class and function
	// pressed binding
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::equipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::crouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::aimButtonPressed);

	//released binding
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ABlasterCharacter::crouchButtonReleased);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::aimButtonReleased);

	// bind the axis from Edit/ProjectSettings/Input to a specific class and function
	PlayerInputComponent->BindAxis("Move_Forward",this,&ABlasterCharacter::moveForward);
	PlayerInputComponent->BindAxis("Move_Right", this, &ABlasterCharacter::moveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::turn);
	PlayerInputComponent->BindAxis("Look_Up", this, &ABlasterCharacter::lookUp);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (combat)
	{
		combat->character = this;
	}
}

// Function used to move the character in forward/backward direction
void ABlasterCharacter::moveForward(float value)
{
	if (Controller != nullptr && value != 0.0f)
	{
		const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(direction, value);
	}
}

//function used to move the character in side direction
void ABlasterCharacter::moveRight(float value)
{
	if (Controller != nullptr && value != 0.0f)
	{
		const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(direction, value);
	}
}

//function used to turn the camera right and left
void ABlasterCharacter::turn(float value)
{
	AddControllerYawInput(value);
}

//function used to turn the camera up and down
void ABlasterCharacter::lookUp(float value)
{
	AddControllerPitchInput(value);
}

void ABlasterCharacter::equipButtonPressed()
{
	if (combat)
	{
		if (HasAuthority())
		{
			combat->equipWeapon(overlappingWeapon);
		}
		else
		{
			serverEquipButtonPress();
		}
		
	}
}

void ABlasterCharacter::crouchButtonPressed()
{
	if (!bIsCrouched)
	{
		Crouch();
	}
	

}

void ABlasterCharacter::crouchButtonReleased()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void ABlasterCharacter::aimButtonPressed()
{
	if (combat)
	{
		combat->setAiming(true);
	}
}

void ABlasterCharacter::aimButtonReleased()
{
	if (combat)
	{
		combat->setAiming(false);
	}
}

void ABlasterCharacter::aimOffset(float deltaTime)
{
	if (combat && combat->equippedWeapon == nullptr)
	{
		startingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		return; // if no weapon equipped exit function
	}
	
	FVector velocity = GetVelocity();
	//setting velocity on z axes (vertical) to 0 as no relevant to determine velocity
	velocity.Z = 0.0f;
	float speed = velocity.Size();

	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (speed == 0.0f && !bIsInAir) // standing still and not jumping
	{
		FRotator currentAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		FRotator deltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentAimRotation, startingAimRotation);
		AO_Yaw = deltaAimRotation.Yaw;
		bUseControllerRotationYaw = false;
	}
	if (speed > 0.0f && !bIsInAir) // running or jumping
	{
		startingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		AO_Yaw = 0.0f;
		bUseControllerRotationYaw = true;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.0f && !IsLocallyControlled())
	{
		// when pitch is sent to server from cleint is converted to 0-360, needs to be converted back
		// map pitch from [270, 360) to [-90,0)
		FVector2D inRange(270.0f, 360.0f);
		FVector2D outRange(-90.0f, 0.0f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(inRange, outRange, AO_Pitch);
	}
}

void ABlasterCharacter::onRep_OverlappingWeapon(AWeaponMaster* lastWeapon)
{
	if (overlappingWeapon)
	{
		overlappingWeapon->showPickupWidget(true);
	}
	else if (lastWeapon)
	{
		lastWeapon->showPickupWidget(false);
	}
}

void ABlasterCharacter::serverEquipButtonPress_Implementation()
{
	if (combat)
	{
		combat->equipWeapon(overlappingWeapon);
	}
}

void ABlasterCharacter::setOverlappingWeapon(AWeaponMaster* weapon)
{
	if (overlappingWeapon)
	{
		overlappingWeapon->showPickupWidget(false);
	}
	overlappingWeapon = weapon;
	if (IsLocallyControlled())
	{
		if (overlappingWeapon)
		{
			overlappingWeapon->showPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::isWeaponEquipped()
{
	// return true only if combat is not null and if equipped weapon is not null
	return (combat && combat->equippedWeapon);
}

bool ABlasterCharacter::isAiming()
{
	// return true only if combat is not null and if the character is aiming
	return (combat && combat->isAiming);
}

