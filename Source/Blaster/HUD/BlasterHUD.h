// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"


// struct that keep track of the textures for the weapon crosshairs
USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* crosshairsCenter;
	UTexture2D* crosshairsLeft;
	UTexture2D* crosshairsRight;
	UTexture2D* crosshairsTop;
	UTexture2D* crosshairsBottom;
	float crosshairSpread{0};
	FLinearColor crosshairColor{ FLinearColor::White};
};

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
public:
	// function used to draw textures on the screen
	virtual void DrawHUD() override;
	//value that need to be set in the blueprints to create the widget
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	TSubclassOf<class UUserWidget> characterOverlayClass{};
	// pointer to the widget that take care of the player stats
	class UCharacterOverlay* characterOverlay{};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// function used to add the widget on screen
	void addCharacterOverlay();

private:
	// reference to the struct that keep track of the crosshairs
	FHUDPackage hudPackage;
	// function used to draw the current crosshair
	void drawCrosshair(UTexture2D* texture, FVector2D viewportCenter,FVector2D spread, FLinearColor crosshairColor);
	// the max spread of the crosshairs
	UPROPERTY(EditAnywhere)
	float crosshairSpreadMax = 16.0f;
public: 
	FORCEINLINE void setHudPackage(const FHUDPackage& package) { hudPackage = package; }
};
