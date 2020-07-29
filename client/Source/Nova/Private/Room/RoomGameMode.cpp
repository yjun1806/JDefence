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
	//// Tick을 작동 시킬건지 아닌지
	//PrimaryActorTick.bCanEverTick = true;

	if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
	{
		// 방에 들어오면 방 번호 전송
		//UNovaGameInstance::NovaSocketcli->SendRoomInfo();

		// 게임방에 들어오면 상태코드를 방번호로 바꿔준다.
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



