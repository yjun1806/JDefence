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
	//로비에 접속할떄 '로비에 접속했다'라는 정보를 서버에 전송
	UNovaGameInstance::NowPlayStateCode = 0; // 로비에 들어오면, 상태 코드를 로비로 바꿔준다. 로비 0
	UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode NowCode : %d"), UNovaGameInstance::NowPlayStateCode);
	gameInstance->NovaSocketcli->SendGameState(UNovaGameInstance::NowPlayStateCode);

	UNovaGameInstance::PlayingGameUserList.Empty();
	//UNovaGameInstance::MyRoomInfo->;
	UNovaGameInstance::TempRoomInfo.Empty();

}


// 현재는 게임종료버튼이지만, 로그아웃 버튼으로 바꿀 예정
void ALobbyGameMode::GameLogOut()
{
	// 모든 로그아웃 처리는 게임인스턴스에서 진행한다.


	////로그아웃했다는 코드정보를 서버에 전송
	//UNovaGameInstance::NowPlayStateCode = UNovaGameInstance::State::LOGOUT; // 우선 상태코드를 바꿔준다.
	//UNovaGameInstance::NovaSocketcli->SendGameState(UNovaGameInstance::NowPlayStateCode);
	////그리고 소켓연결을 해제한다.
	//bool dis = UNovaGameInstance::NovaSocketcli->DisConnect_Server();
	//UE_LOG(LogTemp, Warning, TEXT("DisConnect Server : [ %d ] (1: true, 0: false)"), dis);
	//UNovaGameInstance::Shutdown();

}

