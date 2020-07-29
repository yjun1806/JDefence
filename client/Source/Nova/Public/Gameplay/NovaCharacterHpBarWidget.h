// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "NovaCharacter.h"
#include "NovaCharacterHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UNovaCharacterHpBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY()
		class UProgressBar* HpProgressbar;
public:
	UFUNCTION()
	void UpdateProgressBar(const ANovaCharacter* chr);
	
	
};
