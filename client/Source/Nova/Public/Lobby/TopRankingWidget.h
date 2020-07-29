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
		void UpdateRanking(); // 랭킹업데이트 하는 함수

	UFUNCTION()
		void CheckRankData(); // 서버에서 랭킹데이터를 받아오는지 체크하는 함수

};
