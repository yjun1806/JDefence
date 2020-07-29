// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "Blueprint/UserWidget.h"
#include "LobbyPlayerController.h"
#include "NovaGameInstance.h"




ALobbyGameMode::ALobbyGameMode()
{
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	
	
}



void ALobbyGameMode::BeginPlay()
{
	GLog->Log("LobbyGameMode::BeginPlay()");
	Super::BeginPlay();
	gameInstance = Cast<UNovaGameInstance>(GetWorld()->GetGameInstance());
	//�κ� �����ҋ� '�κ� �����ߴ�'��� ������ ������ ����
	UNovaGameInstance::NowPlayStateCode = 0; // �κ� ������, ���� �ڵ带 �κ�� �ٲ��ش�. �κ� 0
	UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode NowCode : %d"), UNovaGameInstance::NowPlayStateCode);
	gameInstance->NovaSocketcli->SendGameState(UNovaGameInstance::NowPlayStateCode);

	UNovaGameInstance::PlayingGameUserList.Empty();
	//UNovaGameInstance::MyRoomInfo->;
	UNovaGameInstance::TempRoomInfo.Empty();

}


// ����� ���������ư������, �α׾ƿ� ��ư���� �ٲ� ����
void ALobbyGameMode::GameLogOut()
{
	// ��� �α׾ƿ� ó���� �����ν��Ͻ����� �����Ѵ�.


	////�α׾ƿ��ߴٴ� �ڵ������� ������ ����
	//UNovaGameInstance::NowPlayStateCode = UNovaGameInstance::State::LOGOUT; // �켱 �����ڵ带 �ٲ��ش�.
	//UNovaGameInstance::NovaSocketcli->SendGameState(UNovaGameInstance::NowPlayStateCode);
	////�׸��� ���Ͽ����� �����Ѵ�.
	//bool dis = UNovaGameInstance::NovaSocketcli->DisConnect_Server();
	//UE_LOG(LogTemp, Warning, TEXT("DisConnect Server : [ %d ] (1: true, 0: false)"), dis);
	//UNovaGameInstance::Shutdown();

}

