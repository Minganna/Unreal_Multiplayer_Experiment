// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponMaster.generated.h"

// enum that shows weapon current state
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	// weapon on the ground waiting to be picked up
	EWS_Initial UMETA(DisplayName ="Initial State"),
	// weapon equipped by a player
	EWS_Equipped UMETA(DisplayName = "Equipped State"),
	// weapon was dropped by player
	EWS_Dropped UMETA(DisplayName = "Dropped State"),
	// used to display length of enum
	EWS_Max UMETA(DisplayName = "DefaultMax"),
};

UCLASS()
class BLASTER_API AWeaponMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponMaster();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// function used to replicate to the clients variable set on the server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//function used to show/hide the pick up widget text
	void showPickupWidget(bool bShowWidget);
	// function called when the weapon is fired
	virtual void fire(const FVector& hitTarget);
	//function called when dropping the weapon
	void dropped();

	//automatic fire

	// boolean that keep track if the weapon should fire again after a period of time
	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bIsAutomatic{ true };
	// time that should pass before the weapon fires again
	UPROPERTY(EditAnywhere, Category = "Combat")
	float fireDelay{ 0.15 };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// delegate function that is called when game object overlap with this object
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);
	// function used to detect when player stops overllaping with weapon
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);
private:
	// the weapon mesh, used to see the weapon
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* weaponMesh;
	// an invisible spere used to determine if the player is in range to allow pick up
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* areaSphere;


	// variable used to define the current state of the weapon
	UPROPERTY(ReplicatedUsing = onRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState weaponState;

	// widget used to show if the weapon can be picked up depending on state
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* pickupWidget;
	//funtion used to notify clients when state changes on server 
	UFUNCTION()
	void onRep_WeaponState();

	//the weapon animation
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* fireAnimation;
	// the bulletShell object mesh
	UPROPERTY(EditAnywhere, Category = "Casing Properties")
	TSubclassOf<class ACasing> casingClass;
	// random value to add when spawning the casing
	UPROPERTY(EditAnywhere, Category = "Casing Properties")
	float randRangeEject=10.0f;

	// Textures for the weapon crosshairs
	UPROPERTY(EditAnywhere, Category = "Crosshair Properties")
	UTexture2D* crosshairsCenter;
	UPROPERTY(EditAnywhere, Category = "Crosshair Properties")
	UTexture2D* crosshairsLeft;
	UPROPERTY(EditAnywhere, Category = "Crosshair Properties")
	UTexture2D* crosshairsRight;
	UPROPERTY(EditAnywhere, Category = "Crosshair Properties")
	UTexture2D* crosshairsTop;
	UPROPERTY(EditAnywhere, Category = "Crosshair Properties")
	UTexture2D* crosshairsBottom;

	// Zoomed field of view while aiming
	// the default field of view when aiming
	UPROPERTY(EditAnywhere, Category = "Zoom Properties")
	float zoomedFOV{ 30.0f };
	// the default speed which the weapon should zoom the field of view when aiming
	UPROPERTY(EditAnywhere, Category = "Zoom Properties")
	float zoomInterpSpeed{ 20.0f };


public:
	//setter of the weapon state also used to set the necessary variables
	void setWeaponState(EWeaponState state);
	// getter of the areaSphere
	FORCEINLINE USphereComponent* getAreaSphere() const { return areaSphere; }
	// getter for the weapon mesh
	FORCEINLINE USkeletalMeshComponent* getWeaponMesh() const { return weaponMesh; }

	// getter for the textures for the weapon crosshairs
	FORCEINLINE class UTexture2D* getCrosshairCenter() const { return crosshairsCenter; }
	FORCEINLINE UTexture2D* getCrosshairLeft() const { return crosshairsLeft; }
	FORCEINLINE UTexture2D* getCrosshairRight() const { return crosshairsRight; }
	FORCEINLINE UTexture2D* getCrosshairTop() const { return crosshairsTop; }
	FORCEINLINE UTexture2D* getCrosshairBottom() const { return crosshairsBottom; }
	//getter for the zooming variables
	FORCEINLINE float getZoomedFOV() const { return zoomedFOV; }
	FORCEINLINE float getZoomInterpSpeed() const { return zoomInterpSpeed; }

};
