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


	UNovaGameInstance::NovaSocketcli->SendTopRankingRequest(); // ������ ��ŷ������ ��û

	GetWorld()->GetTimerManager().SetTimer(UpdateRankHandler, this, &UTopRankingWidget::CheckRankData, 0.032f, true); // �������� �����Ͱ� �����ϸ� ó���� ������ ����

}

void UTopRankingWidget::UpdateRanking()
{
	// �������� �޾ƿ� �����͸� ������ �¾����ش�.
	for (int i = 0; i < UNovaGameInstance::TopRanker.Num(); i++)
	{
		FString id = UTF8_TO_TCHAR(UNovaGameInstance::TopRanker[i]->Userid.c_str());
		FString point = FString::FromInt(UNovaGameInstance::TopRanker[i]->MaxPoint);
		RankingID[i]->SetText(FText::FromString(id));
		RankingPoint[i]->SetText(FText::FromString(point));
	}
	UNovaGameInstance::UpdateTopRanking = false; // ������Ʈ �÷��׸� ���ش�.
	GetWorld()->GetTimerManager().ClearTimer(UpdateRankHandler); // �����带 ��������ش�.
}

void UTopRankingWidget::CheckRankData()
{
	if (UNovaGameInstance::UpdateTopRanking) // �������� �����Ͱ� �����ϸ� �۵��Ѵ�.
	{
		UpdateRanking();
	}
}
