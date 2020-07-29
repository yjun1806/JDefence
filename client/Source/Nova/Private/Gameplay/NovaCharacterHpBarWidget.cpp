// Fill out your copyright notice in the Description page of Project Settings.

#include "NovaCharacterHpBarWidget.h"
#include "Components/ProgressBar.h"




void UNovaCharacterHpBarWidget::NativeConstruct()
{
	HpProgressbar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPbar_Progress")));
	ABCHECK(HpProgressbar != nullptr);
}

void UNovaCharacterHpBarWidget::UpdateProgressBar(const ANovaCharacter* chr)
{
	//HpProgressbar->SetPercent((chr->CharacterMaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (chr->CharacterCurrentHP/chr->CharacterMaxHP));
	//ABLOG(Error, TEXT("UPPRRR : %f, %f"), chr->CharacterCurrentHP, chr->CharacterMaxHP);
}		
