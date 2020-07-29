// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayGameMode.h"
#include "NovaPlayerController.h"
#include "NovaCharacter.h"
#include "Blueprint/UserWidget.h"
#include "NovaGameInstance.h"
#include "Components/TextBlock.h"


// ���� �����߿� ����ϴ� ���Ӹ���̴�.


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
	
	//UNovaGameInstance::NowPlayStateCode = UNovaGameInstance::State::GAMEPLAY; // �����������̶� �����ڵ带 ����
	// ���� �溰�� �����÷��̸� ������ ���̹Ƿ�, ���� �����ڵ带 ������Ʈ �� �ʿ� ����.
	UE_LOG(LogTemp, Warning, TEXT("APlayGameMode NowCode : %d"), UNovaGameInstance::NowPlayStateCode);
	//UNovaGameInstance::NovaSocketcli->SendGameState(UNovaGameInstance::NowPlayStateCode);
	UNovaGameInstance::NovaSocketcli->SendGameLoadingComplete(UNovaGameInstance::NowPlayStateCode, true);

	// ���ӽð� �ʱ�ȭ �ٽ��ѹ���
	UNovaGameInstance::GameTime = 180;
	UNovaGameInstance::KilledMonsterValue = 0;
	UNovaGameInstance::GameEndLeftTime = 0;
	UNovaGameInstance::GameEndLeftTowerHP = 3000;
	UNovaGameInstance::DiedValue = 0;
	UNovaGameInstance::TotalGaindDamage = 0;
	UNovaGameInstance::TotalReceivedDamage = 0;
	UNovaGameInstance::MyRoomInfo->IsGameEnd = false; //

}




