// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayHUDWidget.h"
#include "Components/TextBlock.h"
#include "NovaGameInstance.h"
#include "Components/BackgroundBlur.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "NovaCharacter.h"
#include "Components/ScaleBox.h"




void UPlayHUDWidget::NativeConstruct()
{
	// �ڽ�Ŭ������ BP_HUD�������Ʈ ���� Ŭ�������� txtPlayerName�̶� �̸��� ���� TextBlock �� ��������ش�.
	playerNameHUD = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
	ABCHECK(nullptr != playerNameHUD);

	playername.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Play_PlayerName_1"))));
	playername.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Play_PlayerName_2"))));
	playername.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Play_PlayerName_3"))));

	Player_border.Add(Cast<UBorder>(GetWidgetFromName(TEXT("Play_Player_1"))));
	Player_border.Add(Cast<UBorder>(GetWidgetFromName(TEXT("Play_Player_2"))));
	Player_border.Add(Cast<UBorder>(GetWidgetFromName(TEXT("Play_Player_3"))));

	CurrentMonNum = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentMonNum")));

	/*PlayerOwnerImg.Add(Cast<UScaleBox>(GetWidgetFromName(TEXT("RoomOwnerImg_1"))));
	PlayerOwnerImg.Add(Cast<UScaleBox>(GetWidgetFromName(TEXT("RoomOwnerImg_2"))));
	PlayerOwnerImg.Add(Cast<UScaleBox>(GetWidgetFromName(TEXT("RoomOwnerImg_3"))));
	PlayerOwnerImg.Add(Cast<UScaleBox>(GetWidgetFromName(TEXT("RoomOwnerImg_4"))));*/

	PlayerHPbar.Add(Cast<UProgressBar>(GetWidgetFromName(TEXT("Play_PlayerHPbar_1"))));
	PlayerHPbar.Add(Cast<UProgressBar>(GetWidgetFromName(TEXT("Play_PlayerHPbar_2"))));
	PlayerHPbar.Add(Cast<UProgressBar>(GetWidgetFromName(TEXT("Play_PlayerHPbar_3"))));
	PlayerHPbar.Add(Cast<UProgressBar>(GetWidgetFromName(TEXT("Play_PlayerHPbar_4"))));


	// ������ �ν��Ͻ��� �����ϱ� ���� �����͸� �������ִ� �κ�
	UNovaGameInstance* gameInstance = Cast<UNovaGameInstance>(GetWorld()->GetGameInstance());
	UE_LOG(LogTemp, Warning, TEXT("TTTTTTTTTT %s"), *FString(gameInstance->userLoginID.c_str()));

	// �����ν��Ͻ����� ���� �α����� �α����̸��� �����ͼ� ������ ����ش�.
	playerNameHUD->SetText(FText::FromString(FString(gameInstance->userLoginID.c_str())));
	//PlayerHPbar[0]->SetPercent

	WaitingPanel = Cast<UBackgroundBlur>(GetWidgetFromName(TEXT("Play_WaitingPanel")));
	ABCHECK(WaitingPanel != nullptr);
	WaitingText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Play_WaitingText")));
	ABCHECK(WaitingText != nullptr);
	WaitingCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("Play_WaitingCount")));
	ABCHECK(WaitingCount != nullptr);


	//SetPlayersHUD();
	Init();
}

void UPlayHUDWidget::PlayStartCounting()
{
	
	if (WaitingCount != nullptr) {
		if (!WaitingCount->IsVisible()) {
			WaitingCount->SetVisibility(ESlateVisibility::Visible);
			SetWaitingText();
		}
		FString tempstr = FString::FromInt(UNovaGameInstance::CountingNumber);
		WaitingCount->SetText(FText::FromString(tempstr));
	}
	
}

void UPlayHUDWidget::RemoveWaitingPanel()
{
	WaitingPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayHUDWidget::WaitingPanelSetBlur()
{
	WaitingPanel->SetBlurStrength(0.0f);
}



void UPlayHUDWidget::SetPlayersHUD()
{
	// ü�°����� ǥ�����ִ� �κ�, �÷��� �ο����� ���� �����ְ�, �̸��� �־��ְ� �ϴ� ����
	int i = 0;
	for (auto& list : UNovaGameInstance::PlayingGameUserList)
	{
		Player_border[i]->SetVisibility(ESlateVisibility::Visible);
		FString tmp = UTF8_TO_TCHAR(list.Value->CharacterName.c_str());
		playername[i]->SetText(FText::FromString(tmp));
		
		i++;
	}
}


void UPlayHUDWidget::UpdateHPbar(const ANovaCharacter* chr)
{
	// ü�°����� ������Ʈ ���ִ� �κ�
	PlayerHPbar[0]->SetPercent(CalculateRatio(UNovaGameInstance::MyCharacterData->CharacterMaxHP, UNovaGameInstance::MyCharacterData->CharacterCurrentHP)); // ���� ĳ���� ������Ʈ

	//ABLOG(Error, TEXT("UHDSFKSL:DF : %s, %d"), UTF8_TO_TCHAR(UNovaGameInstance::MyCharacterData->CharacterID.c_str()), UNovaGameInstance::MyCharacterData->CharacterCurrentHP);
	int i = 1;
	for (auto& list : UNovaGameInstance::PlayingGameUserList)
	{
		//ABLOG(Error, TEXT("HPBar : %s / cu : %f, max : %f"), UTF8_TO_TCHAR(UNovaGameInstance::PlayingGameUserList[i]->CharacterID.c_str()),
			//UNovaGameInstance::PlayingGameUserList[i]->CharacterCurrentHP, UNovaGameInstance::PlayingGameUserList[i]->CharacterMaxHP);

		if (list.Value->CharacterMaxHP <= 0) {
			PlayerHPbar[i]->SetIsEnabled(false);
		}
		else
		{
			PlayerHPbar[i]->SetIsEnabled(true);
		}
		PlayerHPbar[i]->SetPercent(CalculateRatio(list.Value->CharacterMaxHP, list.Value->CharacterCurrentHP));
		i++;
	}
}


float UPlayHUDWidget::CalculateRatio(float MaxHp, float CurrentHP)
{
	return (MaxHp < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / MaxHp);
}

void UPlayHUDWidget::Using_100Potion()
{
	UNovaGameInstance::NovaSocketcli->SendUsingItem(100);
}

void UPlayHUDWidget::Using_50Potion()
{
	UNovaGameInstance::NovaSocketcli->SendUsingItem(50);
}

