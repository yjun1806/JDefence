// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "GameFramework/PlayerController.h"
#include "RoomPlayerController.generated.h"


class URoomWidgetHUD;
/**
 * 
 */
UCLASS()
class NOVA_API ARoomPlayerController : public APlayerController
{
	GENERATED_BODY()

		virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	ARoomPlayerController();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	URoomWidgetHUD* RoomWidgetClass;

	UPROPERTY()
	class URoomWidgetHUD* RoomWidgetHUD;

	UPROPERTY()
	UUserWidget* LoadingScreenClass;

	UPROPERTY()
	class UUserWidget* LoadingScreen;

	void UpdateRoomUserList();
	
	void UpdateRoomChatMessage();

	void StartCounting();

	UFUNCTION(BlueprintCallable, Category = "Chat")
	bool Room_WriteMessageToChatWindow(const FString& msg);
	
};
