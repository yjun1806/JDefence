// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "NovaGameInstance.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()


	
public:


	

	UFUNCTION(BlueprintImplementableEvent, Category = "ReadMesg")
	void UpdateChatmsg(const FText& senderid, const FText& msg);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUserList(const FText& userid, const int32& gamestatecode, const int32& MaxPoint);

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveUserList();

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveRoomList();

	UFUNCTION(BlueprintImplementableEvent)
	void MakeRoomList(const FText& roomName, const int32& roomNumber, const int32& CurrentUserNum, const int32& MaxUserNumber, const bool isthisroomstart, const bool isthisroomlock);

	UFUNCTION(BlueprintImplementableEvent)
		void RefreshInven();
};
