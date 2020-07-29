// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "GameFramework/PlayerController.h"
#include "NovaGameInstance.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

		ALobbyPlayerController();


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	UPROPERTY()
		UUserWidget* LobbyHUDClass;

	UPROPERTY()
		TSubclassOf<class UUserWidget> MessageWidgetClass;

	

	// 블루프린트 위젯을 컨트롤할 변수
	UPROPERTY()
		class UUserWidget* LobbyHUD;

	UPROPERTY()
		class UUserWidget* MessageWidget;




	UFUNCTION(BlueprintCallable, Category = "Chat")
	bool WriteMessageToChatWindow(const FString& msg);

	/*UPROPERTY()
	UNovaGameInstance* gameInstance;*/

	void UpdateNewMessage();

	void UpdateUserList();

	void UpdateRoomList();

	UFUNCTION(BlueprintCallable, Category = "Room")
		void EnterRoom(const int32& roomnumber);

	UFUNCTION(BlueprintCallable, Category = "MakeRoom")
	bool SendRoomInfo(const FText& roomName, const int32& MaxUserNumber, const FText& Password); // 방생성해서 서버에 데이터 보내는 함수

	UPROPERTY()
		UUserWidget* WaitingScreenClass;

	UPROPERTY()
		class UUserWidget* waitingscreen;

};
