// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/WeaponMaster.h"
#include "Blaster/Components/CombatComponent.h"

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

}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind the action (button pressed) from Edit/ProjectSettings/Input to a specific class and function
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::equipButtonPressed);

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


