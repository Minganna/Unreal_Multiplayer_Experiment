// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	//Function to be used in the level blueprints, used to show the widget on start
	UFUNCTION(BlueprintCallable)
	void menuSetup(int32 maxPublicConnection = 4, FString match =FString(TEXT("FreeForAll")),int32 pwd = 0, FString lobbyPath= FString(TEXT("/Game/ThirdPersonCPP/Maps/Lobby")));

protected:
	virtual bool Initialize() override;
	//use this function for unreal 5.0 or under, for 5.1 onwards use the one below
	virtual void OnLevelRemovedFromWorld(ULevel* inLevel, UWorld* inWorld) override;
	//use this function for unreal 5.1 or above, for 5.0 and below use the one above
	//virtual void NativeDestruct() override;

	// Callbacks for the custom delegates on the MultiplayerSubsessionSubSystem
	UFUNCTION()
	void onCreateSession(bool bWasSuccessful);
	//not UFUNCTION because it is not DINAMIC delegate
	void onFindSession(const TArray<FOnlineSessionSearchResult>& sessionResult, bool bWasSuccessful);
	//not UFUNCTION because it is not DINAMIC delegate
	void onJoinSession(EOnJoinSessionCompleteResult::Type result);
	UFUNCTION()
	void onDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void onStartSession(bool bWasSuccessful);
private:
	//the number of players per match, default value to 4
	int32 numPublicConnections{4};
	//the match type, default value to FreeForAll
	FString matchType{TEXT("FreeForAll")};
	//the password for the match
	int32 matchPassword{ 0 };
	FString pathToLobby{TEXT("")};

	//pointer to the subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* multiplayerSessionSubsystem;
	//Bind the widget in C++ to the a button on the widget blueprint (probably found in Plugin content folder)
	//Variable name and button must share the same name
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;
	
	//callback function when a button is clicked
	//needs to be UFUNCTION as they need to be called from blueprints
	UFUNCTION()
	void hostButtonClick();
	UFUNCTION()
	void joinButtonClick();

	void menuTearDown();


};
