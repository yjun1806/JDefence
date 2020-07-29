// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "StoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UStoreWidget : public UUserWidget
{
	GENERATED_BODY()


public:
		UFUNCTION(BlueprintCallable)
		void SendBuyData(int32 Potion100, int32 Potion50, int32 TotalPrice);

	UFUNCTION(BlueprintImplementableEvent)
		void BuyOK();

	UFUNCTION(BlueprintImplementableEvent)
		void BuyFailed();

	UFUNCTION(BlueprintCallable)
		void RefreshItem();


	UPROPERTY()
		FTimerHandle ItemCheckHandler;

	UFUNCTION()
		void CheckBuyItem();
	
};
