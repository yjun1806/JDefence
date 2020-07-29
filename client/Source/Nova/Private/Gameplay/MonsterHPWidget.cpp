// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterHPWidget.h"
#include "Components/ProgressBar.h"




void UMonsterHPWidget::NativeConstruct()
{
	MonsterHpProgressbar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPbar_ProgressMon")));
	ABCHECK(MonsterHpProgressbar != nullptr);
}

void UMonsterHPWidget::UpdateProgressBar(const AMonsterCharacter* mon)
{
	MonsterHpProgressbar->SetPercent((mon->MonsterMaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (mon->MonsterCurrentHP/mon->MonsterMaxHP));
}
