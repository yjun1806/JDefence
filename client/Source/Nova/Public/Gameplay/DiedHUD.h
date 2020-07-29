// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "DiedHUD.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UDiedHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY()
		class UButton* ImmediatelyRevivalBtn;

	UPROPERTY()
		class UButton* RevivalBtn;

	UFUNCTION(BlueprintImplementableEvent)
		void StartCounting();

	UFUNCTION()
		void CharacterRevivalClicked();
	
	UFUNCTION()
		void CharacterRevivalFullHPClicked();
	
};
