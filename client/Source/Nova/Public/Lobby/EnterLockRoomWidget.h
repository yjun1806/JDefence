// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "EnterLockRoomWidget.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UEnterLockRoomWidget : public UUserWidget
{
	GENERATED_BODY()

		virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
		void Enter();

	UFUNCTION(BlueprintCallable)
	void SendPasswordToServer(const FText& password, const int32& roomnumber);
	
	UFUNCTION(BlueprintImplementableEvent)
		void EnterRoomPermissionOK(bool result);

	UPROPERTY()
		FTimerHandle RoomCheckHandler;

	UFUNCTION()
		void CheckReceivedData();
	
};
