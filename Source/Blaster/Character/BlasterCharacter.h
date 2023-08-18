// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/Types/TurningInPlace.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// function used to replicate to the clients variable set on the server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// function called as soon as the components are initialised
	virtual void PostInitializeComponents() override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Function used to move the character in forward/backward direction
	void moveForward(float value);
	//function used to move the character in side direction
	void moveRight(float value);
	//function used to turn the camera right and left
	void turn(float value);
	//function used to turn the camera up and down
	void lookUp(float value);
	// function used to detect if the equip input has been pressed
	void equipButtonPressed();
	// function used to detect if the crouch input has been pressed
	void crouchButtonPressed();
	// function used to detect if the crouch input has been released
	void crouchButtonReleased();
	// function used to detect if the aim input has been pressed
	void aimButtonPressed();
	// function used to detect if the aim input has been released
	void aimButtonReleased();
	// function used to calculate the yaw and pitch offset
	void aimOffset(float deltaTime);
	//Add possibility to override the Jump function
	virtual void Jump() override;

	//Grappling logic
	//initialise the grapple mesh
	void initialiseGrappleMesh();
	//Blueprint variable used to move towards a object, this is a blueprint event remembe to check blueprint implementation
	UFUNCTION(BlueprintImplementableEvent)
	void tryGrappling(FVector attachPoint);
	//spawn the rope/tongue object
	void spawnGrappleMesh();
	// function called from blueprint when the character reached the destination
	UFUNCTION(BlueprintCallable)
	void destroyGrappleMesh();
private:
	// class that allow the camera to follow the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* springArm;
	//the main camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* mainCamera;
	//Widget that display information on top of the character's head
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;
	// used to store a reference to the weapon the character is colliding from (that can be picked up)
	UPROPERTY(ReplicatedUsing = onRep_OverlappingWeapon)
	class AWeaponMaster* overlappingWeapon;
	// used to store a reference to the current attachPoint
	UPROPERTY(Replicated)
	FVector travelPoint;
	// function called when overlappingWeapon get replicated
	UFUNCTION()
	void onRep_OverlappingWeapon(AWeaponMaster* lastWeapon);
	//component used to take care of all the combat logic of the player
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* combat;
	// remote procedure call function that allow a client to request the server to equip a weapon
	UFUNCTION(Server, Reliable)
	void serverEquipButtonPress();
	// variable used to determine the aim offset yaw rotation
	float AO_Yaw;
	// value used to check if the character needs to rotate the root bone
	float interpAO_Yaw;
	// variable used to determine the aim offset pitch rotation
	float AO_Pitch;
	// used to keep track of the rotation when not moving
	FRotator startingAimRotation;
	// enum used to detect if the camera reached a certain point and the character needs to start turning in place
	ETurningInPlace turningInPlace;
	// function used to determine if the character should turn in place base by the offset yaw
	void turnInPlace(float deltaTime);
	// the tongue/rope blueprint object
	UBlueprint* BP_grappleMeshToSpawn;


	//Grappling logic
	// remote procedure call function that allow a client to request the server to grapple
	UFUNCTION(Server, Reliable)
	void serverTryGrapple();
	// destroy the grapple mesh for a client
	UFUNCTION(Server, Reliable)
	void serverDestroyGrappleMesh();
	// function called when the tongue/rope object get replicated and it requires to be set the start end pos
	UFUNCTION()
	void onRep_GrappleMesh();
	// the tongue/rope object
	UPROPERTY(ReplicatedUsing = onRep_GrappleMesh)
	class AGrapplinMesh* grappleMeshToSpawn;
	//transform location where the grapple will spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* grappleSpawnLocation;


public:	
	// setter for overlappingWeapon variable
	void setOverlappingWeapon(AWeaponMaster* weapon);
	// getter for the weapon equipped variable
	bool isWeaponEquipped();
	//getter for the aiming bool
	bool isAiming();
	//getter for the aim offset yaw
	FORCEINLINE float getAO_YAW() const { return AO_Yaw; }
	//getter for the aim offset pitch
	FORCEINLINE float getAO_Pitch() const { return AO_Pitch; }
	//getter for turning in place
	FORCEINLINE ETurningInPlace getTurningInPlace() const { return turningInPlace; }
	//getter for the camera
	FORCEINLINE UCameraComponent* getMainCamera() const { return mainCamera; }
	//getter of the equipped weapon
	AWeaponMaster* getEquippedWeapon();

	//Grappling logic
	//getter for the camera
	FORCEINLINE USceneComponent* getGrappleTransform() const { return grappleSpawnLocation; }
};
