// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D viewportSize;

	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
		const FVector2D vieportCenter(viewportSize.X/2.0f, viewportSize.Y / 2.0f);

		float spreadScaled = crosshairSpreadMax * hudPackage.crosshairSpread;

		if (hudPackage.crosshairsCenter)
		{
			FVector2D spread(0.0f, 0.0f);
			drawCrosshair(hudPackage.crosshairsCenter, vieportCenter, spread);
		}
		if (hudPackage.crosshairsLeft)
		{
			FVector2D spread(-spreadScaled, 0.0f);
			drawCrosshair(hudPackage.crosshairsLeft, vieportCenter,spread);
		}
		if (hudPackage.crosshairsRight)
		{
			FVector2D spread(spreadScaled, 0.0f);
			drawCrosshair(hudPackage.crosshairsRight, vieportCenter, spread);
		}
		if (hudPackage.crosshairsTop)
		{
			FVector2D spread(0.0f, -spreadScaled);
			drawCrosshair(hudPackage.crosshairsTop, vieportCenter, spread);
		}
		if (hudPackage.crosshairsBottom)
		{
			FVector2D spread(0.0f, spreadScaled);
			drawCrosshair(hudPackage.crosshairsBottom, vieportCenter,spread);
		}

	}

}

void ABlasterHUD::drawCrosshair(UTexture2D* texture, FVector2D viewportCenter, FVector2D spread)
{
	
	const float textureWidth = texture->GetSizeX();
	const float textureHeight = texture->GetSizeY();
	// ensure the texture is centered, otherwise there would be an offset due to pixel 0 been the top right corner
	const FVector2D textureDrawPoint(viewportCenter.X - (textureWidth / 2) + spread.X, viewportCenter.Y - (textureHeight / 2)+ spread.Y);

	DrawTexture(texture, textureDrawPoint.X, textureDrawPoint.Y, textureWidth, textureHeight,0.0f,0.0f,1.0f,1.0f,FLinearColor::White);
}
