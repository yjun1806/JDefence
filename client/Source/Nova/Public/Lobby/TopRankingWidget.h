// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "TopRankingWidget.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UTopRankingWidget : public UUserWidget
{
	GENERATED_BODY()

		virtual void NativeConstruct() override;

	
public:
	UPROPERTY()
		TArray<class UTextBlock*> RankingID;

	UPROPERTY()
		TArray<class UTextBlock*> RankingPoint;
	
	UPROPERTY()
	FTimerHandle UpdateRankHandler;

	UFUNCTION()
		void UpdateRanking(); // ��ŷ������Ʈ �ϴ� �Լ�

	UFUNCTION()
		void CheckRankData(); // �������� ��ŷ�����͸� �޾ƿ����� üũ�ϴ� �Լ�

};
