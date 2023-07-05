// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

//Declaring our own custom delegates for the menu class to bind callbacks to
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
//in unreal 5.0 or over it might be needed to give the params a name
//names: sessionResult && bWasSuccessful
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>&, bool);
//name: result
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroyingSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartingSessionComplete, bool, bWasSuccessful);
/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionsSubsystem();

	// our own custom delegate for the menu class to bind callbacks to
	FMultiplayerOnCreateSessionComplete multiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete multiplayerOnFindSessionComplete;
	FMultiplayerOnJoinSessionComplete multiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroyingSessionComplete multiplayerOnDestroySessionComplete;
	FMultiplayerOnStartingSessionComplete multiplayerOnStartSessionComplete;
	//To handle sessions functionality. the menu class will call these
	// create game session
	// param[in] numPublicConnection : max number of player for the match
	// param[in] matchType : the type of match of the multiplayer game, example -> capture the flag
	// param[in] pwd : the password to allow player to play with friends
	void createSession(int32 numPublicConnection,FString matchType, int32 pwd);
	// Find game session
	// param[in] maxSearchResult : the max number of search the delegate will have
	void findSession(int32 maxSearchResult);
	// Join game session
	// param[in] the first available session that can be joined
	void joinSession(const FOnlineSessionSearchResult& sessionResult);
	// Destroy game session
	void destroySession();
	// Start game session
	void startSession();

protected:
	//internal callback for the delegates we'll add to the Online session interface delegate list
	// these won't need to be called outside this class
	//Callback function used to notify when the delegate has finished to create the game session
	//param[in] sessionName : the name of the gameSession
	//param[in] bWasSuccessful : bool used to confirm if the session was created successfully
	void onCreateSessionComplete(FName sessionName, bool bWasSuccessful);
	//Callback function used to notify when the delegate has found an available game session
	//param[in] bWasSuccessful : bool used to confirm if the session was found successfully
	void onFindSessionComplete(bool bWasSuccessful);
	//Callback function used to notify when the delegate has finished joining the session
	//param[in] sessionName : the name of the gameSession
	//param[in] result : used to define if the game session was joined
	void onJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	//Callback function used to notify when the delegate has finished destroing the session
	//param[in] sessionName : the name of the gameSession
	//param[in] bWasSuccessful : bool used to confirm if the session was destroyed successfully
	void onDestroySessionComplete(FName sessionName, bool bWasSuccessful);
	//Callback function used to notify when the delegate has finished starting the session
	//param[in] sessionName : the name of the gameSession
	//param[in] bWasSuccessful : bool used to confirm if the session was started successfully
	void onStartSessionComplete(FName sessionName, bool bWasSuccessful);
private:
	//pointer to the online session interface
	IOnlineSessionPtr sessionInterface;

	TSharedPtr<FOnlineSessionSettings> lastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> lastSessionSearch;

	//boolean used to check if the last session need to be destroyed
	bool bOnCreateSessionOnDestroy{ false };
	//save the last game params
	int32 lastNumPublicConnection;
	FString lastMatchType;
	int32 lastPwd {0};

	//To add to the online session interface delegate list
	// We'll bind our MultiplayerSessionsSubsystem internal callback to these.
	//delegate used to create an online session, initialised in the constructor
	FOnCreateSessionCompleteDelegate createSessionCompleteDelegate;
	//delegate used to find an online session, initialised in the constructor
	FOnFindSessionsCompleteDelegate findSessionCompleteDelegate;
	//delegate used to join a session when found, initialised in the constructor
	FOnJoinSessionCompleteDelegate joinSessionCompleteDelegate;
	//delegate used to destroy a session, initialised in the constructor
	FOnDestroySessionCompleteDelegate destroySessionCompleteDelegate;
	//delegate used to start a session, initialised in the constructor
	FOnStartSessionCompleteDelegate startSessionCompleteDelegate;

	//delegate handles
	FDelegateHandle createSessionCompleteDelegateHandle;
	FDelegateHandle findSessionCompleteDelegateHandle;
	FDelegateHandle joinSessionCompleteDelegateHandle;
	FDelegateHandle destroySessionCompleteDelegateHandle;
	FDelegateHandle startSessionCompleteDelegateHandle;
};
