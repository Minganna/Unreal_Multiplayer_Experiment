// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"



void UMenu::menuSetup(int32 maxPublicConnection, FString match, int32 pwd, FString lobbyPath)
{
	pathToLobby = FString::Printf(TEXT("%s?listen"), *lobbyPath);
	numPublicConnections = maxPublicConnection;
	matchType = match;
	//add the menu widget to the viewport
	AddToViewport();
	//make the widget visible on screen
	SetVisibility(ESlateVisibility::Visible);
	//make the game focus only on the widget o that the character can not move around
	bIsFocusable = true;

	UWorld* world = GetWorld();

	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			playerController->SetInputMode(inputModeData);
			//remove in unreal 5
			//playerController->bShowMouseCursor = true;
			//Unreal 5 might use the following
			playerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		multiplayerSessionSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (multiplayerSessionSubsystem)
	{
		multiplayerSessionSubsystem->multiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::onCreateSession);
		multiplayerSessionSubsystem->multiplayerOnFindSessionComplete.AddUObject(this, &ThisClass::onFindSession);
		multiplayerSessionSubsystem->multiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::onJoinSession);
		multiplayerSessionSubsystem->multiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::onDestroySession);
		multiplayerSessionSubsystem->multiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::onStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMenu::hostButtonClick);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::joinButtonClick);
	}
	return true;
}
//use this function for unreal 5.0 or under, for 5.1 onwards use the one below
void UMenu::OnLevelRemovedFromWorld(ULevel * inLevel, UWorld* inWorld)
{
	menuTearDown();
	Super::OnLevelRemovedFromWorld(inLevel, inWorld);
}
//use this function for unreal 5.1 or above, for 5.0 and below use the one above
/*
void UMenu::NativeDestruct()
{
	menuTearDown();
	Super::NativeDestruct();
}
*/

void UMenu::onCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Blue,
				FString::Printf(TEXT("Session created successfully"))
			);
		}
		if (multiplayerSessionSubsystem)
		{
			multiplayerSessionSubsystem->startSession();
		}
		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel(pathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Red,
				FString::Printf(TEXT("Failed to create Session"))
			);
		}
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::onFindSession(const TArray<FOnlineSessionSearchResult>& sessionResult, bool bWasSuccessful)
{
	if (multiplayerSessionSubsystem == nullptr)
	{
		return;
	}
	for (auto result : sessionResult)
	{
		//check if matching key pair for match session
		FString settingsValue;
		result.Session.SessionSettings.Get(FName("Match Type"), settingsValue);
		int32 sessionPwd;
		result.Session.SessionSettings.Get(FName("Password"), sessionPwd);
		//check if is the same match type 
		if (settingsValue == matchType && sessionPwd == matchPassword)
		{
			multiplayerSessionSubsystem->joinSession(result);
			return;
		}

	}
	if (!bWasSuccessful || sessionResult.Num() == 0)
	{
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::onJoinSession(EOnJoinSessionCompleteResult::Type result)
{
	IOnlineSubsystem* subSystem = IOnlineSubsystem::Get();
	if (subSystem)
	{
		IOnlineSessionPtr sessionInterface = subSystem->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			//find the correct ip address to join
			FString address;
			sessionInterface->GetResolvedConnectString(NAME_GameSession, address);

			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				//join another player server
				playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	if (result != EOnJoinSessionCompleteResult::Success)
	{
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::onDestroySession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Red,
				FString::Printf(TEXT("Destroyed session successfully"))
			);
		}

	}
}

void UMenu::onStartSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Yellow,
				FString::Printf(TEXT("Started session successfully"))
			);
		}

	}
}

void UMenu::hostButtonClick()
{
	HostButton->SetIsEnabled(false);
	JoinButton->SetIsEnabled(true);
	if (multiplayerSessionSubsystem)
	{
		multiplayerSessionSubsystem->createSession(numPublicConnections,matchType, matchPassword);
	}
}

void UMenu::joinButtonClick()
{
	HostButton->SetIsEnabled(false);
	JoinButton->SetIsEnabled(false);
	if (multiplayerSessionSubsystem)
	{
		//set to a 10000 as currently using game id of steam example
		multiplayerSessionSubsystem->findSession(10000);
	}
}

void UMenu::menuTearDown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeGameOnly inputModeData;
			playerController->SetInputMode(inputModeData);
			//playerController->bShowMouseCursor = false;
			//Unreal 5 might use the following
			playerController->SetShowMouseCursor(false);
		}
	}
}
