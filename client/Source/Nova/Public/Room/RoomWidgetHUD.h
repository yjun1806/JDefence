// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "RoomWidgetHUD.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API URoomWidgetHUD : public UUserWidget
{
	GENERATED_BODY()

		virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TArray<class UTextBlock*> UserIDText;

	UPROPERTY()
	TArray<class UBorder*> UserSlot;

	UPROPERTY()
	TArray<class UBorder*> ReadySlot;

	UPROPERTY()
	TArray<class UTextBlock*> UserMP;

	UPROPERTY()
	class UButton* Readybtn;

	UPROPERTY()
	class UButton* ReadyCancelbtn;

	UPROPERTY()
	class UButton* Startbtn;

	UPROPERTY()
	class UButton* LeaveRoomToLobby;

	UPROPERTY()
	class UEditableTextBox* RoomChatUserId;

	UPROPERTY()
	class UBorder* CountingPanel;

	UPROPERTY()
	class UTextBlock* CountingText;

	UPROPERTY()
		class UImage* LockImg;


public:
	UPROPERTY()
	class UTextBlock* RoomName;
	UPROPERTY()
	class UTextBlock* RoomNumber;

	UFUNCTION(BlueprintCallable)
	//void UpdateRMUserList(const FText& userid, bool isready);
	void UpdateRMUserList();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateRoomChatmsg(const FText& senderid, const FText& msg);
	
	UFUNCTION(BlueprintImplementableEvent)
		void GetGameData();

	UFUNCTION(BlueprintCallable)
		void GameReady();

	UFUNCTION(BlueprintCallable)
		void GameReadyCancel();

	UFUNCTION(BlueprintCallable)
		void LeaveGameRoom();

	UFUNCTION()
		void StartGamePlay();

	UFUNCTION()
	void StartCounting();

};
