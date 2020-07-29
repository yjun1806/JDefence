// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomGameMode.h"
#include "RoomWidgetHUD.h"
#include "NovaGameInstance.h"
#include "RoomPlayerController.h"
#include "Blueprint/UserWidget.h"




ARoomGameMode::ARoomGameMode()
{
	
	PlayerControllerClass = ARoomPlayerController::StaticClass();

}

void ARoomGameMode::BeginPlay()
{
	Super::BeginPlay();
	//// Tick�� �۵� ��ų���� �ƴ���
	//PrimaryActorTick.bCanEverTick = true;

	if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
	{
		// �濡 ������ �� ��ȣ ����
		//UNovaGameInstance::NovaSocketcli->SendRoomInfo();

		// ���ӹ濡 ������ �����ڵ带 ���ȣ�� �ٲ��ش�.
		UNovaGameInstance::NowPlayStateCode = UNovaGameInstance::MyRoomInfo->RoomNumber;
		UNovaGameInstance::NovaSocketcli->SendGameState(UNovaGameInstance::NowPlayStateCode);
		UE_LOG(LogTemp, Warning, TEXT("Owner RoomGameMode NowCode : %d"), UNovaGameInstance::NowPlayStateCode);
	}
	else 
	{
		UNovaGameInstance::NowPlayStateCode = UNovaGameInstance::MyRoomInfo->RoomNumber;
		UNovaGameInstance::NovaSocketcli->SendGameState(UNovaGameInstance::NowPlayStateCode);
		UE_LOG(LogTemp, Warning, TEXT("Client RoomGameMode NowCode : %d"), UNovaGameInstance::NowPlayStateCode);
	}
	
}



