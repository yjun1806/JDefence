// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "MonsterCharacter.h"
#include "MonsterHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UMonsterHPWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
		class UProgressBar* MonsterHpProgressbar;
public:
	UFUNCTION()
		void UpdateProgressBar(const AMonsterCharacter* mon);
	
	
};
