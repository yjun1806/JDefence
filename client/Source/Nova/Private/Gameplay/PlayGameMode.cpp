// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayGameMode.h"
#include "NovaPlayerController.h"
#include "NovaCharacter.h"
#include "Blueprint/UserWidget.h"
#include "NovaGameInstance.h"
#include "Components/TextBlock.h"


// 게임 진행중에 사용하는 게임모드이다.


APlayGameMode::APlayGameMode()
{

	PlayerControllerClass = ANovaPlayerController::StaticClass();
	

	//DefaultPawnClass = ANovaCharacter::StaticClass();

	/*static ConstructorHelpers::FClassFinder<ANovaCharacter> PlayerPawnObject(TEXT(
		"/Game/NovaProject/Blueprints/BP_NovaCharacter.BP_NovaCharacter_C"));

	if (PlayerPawnObject.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnObject.Class;
	}*/

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT(
		"/Game/NovaProject/Blueprints/BP_Character.BP_Character_C"));

	if (PlayerPawnObject.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnObject.Class;
	}

}

void APlayGameMode::BeginPlay()
{
	
	//UNovaGameInstance::NowPlayStateCode = UNovaGameInstance::State::GAMEPLAY; // 게임진행중이란 상태코드를 전송
	// 이제 방별로 게임플레이를 구분할 것이므로, 굳이 상태코드를 업데이트 할 필요 없다.
	UE_LOG(LogTemp, Warning, TEXT("APlayGameMode NowCode : %d"), UNovaGameInstance::NowPlayStateCode);
	//UNovaGameInstance::NovaSocketcli->SendGameState(UNovaGameInstance::NowPlayStateCode);
	UNovaGameInstance::NovaSocketcli->SendGameLoadingComplete(UNovaGameInstance::NowPlayStateCode, true);

	// 게임시간 초기화 다시한번더
	UNovaGameInstance::GameTime = 180;
	UNovaGameInstance::KilledMonsterValue = 0;
	UNovaGameInstance::GameEndLeftTime = 0;
	UNovaGameInstance::GameEndLeftTowerHP = 3000;
	UNovaGameInstance::DiedValue = 0;
	UNovaGameInstance::TotalGaindDamage = 0;
	UNovaGameInstance::TotalReceivedDamage = 0;
	UNovaGameInstance::MyRoomInfo->IsGameEnd = false; //

}




