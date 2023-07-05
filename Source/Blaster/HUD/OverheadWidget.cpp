// Fill out your copyright notice in the Description page of Project Settings.

#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::setDisplayText(FString textToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(textToDisplay));
	}
}

void UOverheadWidget::showPlayerNetRole(APawn* inPawn)
{
	ENetRole remoteRole = inPawn->GetRemoteRole();
	FString role;
	switch (remoteRole)
	{
	case ENetRole::ROLE_Authority:
		role = FString("Autority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		role = FString("None");
		break;
	default:
		role = FString("Default / error");
		break;
	}

	FString remoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *role);
	setDisplayText(remoteRoleString);
}

void UOverheadWidget::getPlayerName(APawn* inPawn)
{
	// get the player state
	const APlayerState* playerState = inPawn->GetPlayerState();

	if (!playerState || !*playerState->GetPlayerName() && totalTime <getPlayerNameTimeOut)
	{
		FTimerHandle getPlayerStateTimer;
		FTimerDelegate tryAgainDelegate;
		tryAgainDelegate.BindUFunction(this, FName("getPlayerName"), inPawn);
		GetWorld()->GetTimerManager().SetTimer(getPlayerStateTimer, tryAgainDelegate, getPlayerNameInterval, false, 0.1f);
		totalTime += getPlayerNameInterval;
		return;
	}
	const FString playerName = playerState->GetPlayerName();
	setDisplayText(playerName);


}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* inLevel, UWorld* inWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(inLevel, inWorld);
}
