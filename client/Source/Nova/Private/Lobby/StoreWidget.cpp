// Fill out your copyright notice in the Description page of Project Settings.

#include "StoreWidget.h"
#include "NovaGameInstance.h"
#include "TimerManager.h"







void UStoreWidget::SendBuyData(int32 Potion100, int32 Potion50, int32 TotalPrice)
{
	UNovaGameInstance::NovaSocketcli->SendBuyData(Potion100, Potion50, TotalPrice);
	GetWorld()->GetTimerManager().SetTimer(ItemCheckHandler, this, &UStoreWidget::CheckBuyItem, 0.032f, true); // �������� �����Ͱ� �����ϸ� ó���� ������ ����

}

void UStoreWidget::RefreshItem()
{
	UNovaGameInstance::NovaSocketcli->SendRefreshItemData();
}

void UStoreWidget::CheckBuyItem()
{
	if (UNovaGameInstance::ItemBuySucessful == 1) { // ���Ž���
		BuyFailed();
		UNovaGameInstance::ItemBuySucessful = 0;
		GetWorld()->GetTimerManager().ClearTimer(ItemCheckHandler);

	}
	else if(UNovaGameInstance::ItemBuySucessful == 2) // ���ż���
	{
		BuyOK();
		UNovaGameInstance::ItemBuySucessful = 0;
		GetWorld()->GetTimerManager().ClearTimer(ItemCheckHandler);

	}

}
