// Fill out your copyright notice in the Description page of Project Settings.

#include "TopRankingWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "NovaGameInstance.h"




void UTopRankingWidget::NativeConstruct()
{
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_1_ID"))));
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_2_ID"))));
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_3_ID"))));
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_4_ID"))));
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_5_ID"))));
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_6_ID"))));
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_7_ID"))));
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_8_ID"))));
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_9_ID"))));
	RankingID.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_10_ID"))));

	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_1_Point"))));
	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_2_Point"))));
	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_3_Point"))));
	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_4_Point"))));
	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_5_Point"))));
	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_6_Point"))));
	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_7_Point"))));
	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_8_Point"))));
	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_9_Point"))));
	RankingPoint.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Top_10_Point"))));


	UNovaGameInstance::NovaSocketcli->SendTopRankingRequest(); // 서버에 랭킹데이터 요청

	GetWorld()->GetTimerManager().SetTimer(UpdateRankHandler, this, &UTopRankingWidget::CheckRankData, 0.032f, true); // 서버에서 데이터가 도착하면 처리할 스레드 생성

}

void UTopRankingWidget::UpdateRanking()
{
	// 서버에서 받아온 데이터를 위젯에 셋업해준다.
	for (int i = 0; i < UNovaGameInstance::TopRanker.Num(); i++)
	{
		FString id = UTF8_TO_TCHAR(UNovaGameInstance::TopRanker[i]->Userid.c_str());
		FString point = FString::FromInt(UNovaGameInstance::TopRanker[i]->MaxPoint);
		RankingID[i]->SetText(FText::FromString(id));
		RankingPoint[i]->SetText(FText::FromString(point));
	}
	UNovaGameInstance::UpdateTopRanking = false; // 업데이트 플래그를 꺼준다.
	GetWorld()->GetTimerManager().ClearTimer(UpdateRankHandler); // 스레드를 종료시켜준다.
}

void UTopRankingWidget::CheckRankData()
{
	if (UNovaGameInstance::UpdateTopRanking) // 서버에서 데이터가 도착하면 작동한다.
	{
		UpdateRanking();
	}
}
