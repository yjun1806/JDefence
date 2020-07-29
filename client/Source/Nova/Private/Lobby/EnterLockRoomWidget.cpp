// Fill out your copyright notice in the Description page of Project Settings.

#include "EnterLockRoomWidget.h"
#include "TimerManager.h"
#include "NovaGameInstance.h"

void UEnterLockRoomWidget::NativeConstruct()
{
}

void UEnterLockRoomWidget::Enter()
{
	GetWorld()->GetTimerManager().SetTimer(RoomCheckHandler, this, &UEnterLockRoomWidget::CheckReceivedData, 0.032f, true); // 서버에서 데이터가 도착하면 처리할 스레드 생성
}

void UEnterLockRoomWidget::SendPasswordToServer(const FText & password, const int32& roomnumber)
{
	UNovaGameInstance::NovaSocketcli->SendPassword(password, roomnumber);
}

void UEnterLockRoomWidget::CheckReceivedData()
{
	if (UNovaGameInstance::ReceivedPasswordCheckData)
	{
		EnterRoomPermissionOK(UNovaGameInstance::IsPasswordCorrect);
		GetWorld()->GetTimerManager().ClearTimer(RoomCheckHandler);
		UNovaGameInstance::ReceivedPasswordCheckData = false;
	}
}
