// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "GameEndWidget.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UGameEndWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class UButton* BackToLobbyBtn;

	UPROPERTY()
	class UButton* BackToLobbyBtnWin;

	UFUNCTION()
		void BackToLobbyFtn();
	
	UFUNCTION(BlueprintImplementableEvent)
		void SetGameOverOrGameWin(int code); // code 0 : GameOver, code 1: GameWin
	
};
