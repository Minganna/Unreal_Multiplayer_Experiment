// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "Engine.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() : createSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::onCreateSessionComplete)),
																 findSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::onFindSessionComplete)),
																 joinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::onJoinSessionComplete)),
																 destroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this,&ThisClass::onDestroySessionComplete)),
																 startSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this,&ThisClass::onStartSessionComplete))
{
	//Code Created to get the current unreal subsystem, currently should return Steam outside the editor and Null in the editor
	IOnlineSubsystem* subSystem = IOnlineSubsystem::Get();
	if (subSystem)
	{
		sessionInterface = subSystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::createSession(int32 numPublicConnection, FString matchType, int32 pwd)
{
	if (!sessionInterface.IsValid())
	{
		return;
	}
	auto existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession != nullptr)
	{
		bOnCreateSessionOnDestroy = true;
		lastNumPublicConnection = numPublicConnection;
		lastMatchType = matchType;
		lastPwd = pwd;
		destroySession();
	}
	//store the delegate in a FDelegateHandle so we can later remove it from the delegate list
	createSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegate);

	lastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	//setting session settings
	//game will connect over the internet therefore it should not connect via Lan
	lastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() =="NULL" ? true : false;
	// maximum number of player per session
	lastSessionSettings->NumPublicConnections = numPublicConnection;
	// can player join when game is already started?
	lastSessionSettings->bAllowJoinInProgress = false;
	//steam uses presence to connect users with players in similar region
	lastSessionSettings->bAllowJoinViaPresence = true;
	// should steam advertise this session to other player so that they can join
	lastSessionSettings->bShouldAdvertise = true;
	// should steam set presence to find players on your region?
	lastSessionSettings->bUsesPresence = true;
	//setting used to specify the match type
	lastSessionSettings->Set(FName("Match Type"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//setting password
	lastSessionSettings->Set(FName("Password"), pwd, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	lastSessionSettings->BuildUniqueId = 1;
	//to avoid failures in creating a session, the creation of a session should only be called when bOnCreateSessionOnDestroy is false
	if (!bOnCreateSessionOnDestroy)
	{
		//if unable to find online sessions use the following (Only available in Unreal engine 4.27 and over)
		lastSessionSettings->bUseLobbiesIfAvailable = true;
		const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (!sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *lastSessionSettings))
		{
			sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);

			//Broadcast our own custom delegate
			multiplayerOnCreateSessionComplete.Broadcast(false);
		}
	}
}

void UMultiplayerSessionsSubsystem::findSession(int32 maxSearchResult)
{
	if (!sessionInterface.IsValid())
	{
		return;
	}

	findSessionCompleteDelegateHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(findSessionCompleteDelegate);
	lastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	lastSessionSearch->MaxSearchResults = maxSearchResult;
	//check what subsystem we are using and set land based on the result
	lastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	lastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), lastSessionSearch.ToSharedRef()))
	{
		//remove the delegate from the delegate list
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionCompleteDelegateHandle);
		//Broadcast search result to the menu system
		multiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(),false);
	}
}

void UMultiplayerSessionsSubsystem::joinSession(const FOnlineSessionSearchResult & sessionResult)
{
	if (!sessionInterface.IsValid())
	{
		multiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	joinSessionCompleteDelegateHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegate);
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, sessionResult))
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
		multiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::destroySession()
{
	if (!sessionInterface.IsValid())
	{
		multiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}
	destroySessionCompleteDelegateHandle = sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegate);

	if (!sessionInterface->DestroySession(NAME_GameSession))
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegateHandle);
		multiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::startSession()
{
	if (!sessionInterface.IsValid())
	{
		multiplayerOnStartSessionComplete.Broadcast(false);
	}
	startSessionCompleteDelegateHandle = sessionInterface->AddOnStartSessionCompleteDelegate_Handle(startSessionCompleteDelegate);
	if (!sessionInterface->StartSession(NAME_GameSession))
	{
		sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(startSessionCompleteDelegateHandle);
		multiplayerOnStartSessionComplete.Broadcast(false);

	}

}

void UMultiplayerSessionsSubsystem::onCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);
	}
	//Broadcast our own custom delegate
	multiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::onFindSessionComplete(bool bWasSuccessful)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionCompleteDelegateHandle);
	}
	if (lastSessionSearch->SearchResults.Num() <= 0)
	{
		multiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}
	multiplayerOnFindSessionComplete.Broadcast(lastSessionSearch->SearchResults,bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::onJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
	}

	multiplayerOnJoinSessionComplete.Broadcast(result);
}

void UMultiplayerSessionsSubsystem::onDestroySessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegateHandle);
	}
	if (bWasSuccessful && bOnCreateSessionOnDestroy)
	{
		bOnCreateSessionOnDestroy = false;
		createSession(lastNumPublicConnection, lastMatchType, lastPwd);
	}
	multiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::onStartSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if(!sessionInterface.IsValid())
	{
		return;
	}
	if (bWasSuccessful)
	{
		sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(startSessionCompleteDelegateHandle);
		multiplayerOnStartSessionComplete.Broadcast(bWasSuccessful);
	}
}
