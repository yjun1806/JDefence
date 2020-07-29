// Fill out your copyright notice in the Description page of Project Settings.

#include "NovaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NovaGameInstance.h"
#include "TimerManager.h"
#include "NovaCharacter.h"
#include "MonsterCharacter.h"
#include "Blueprint/UserWidget.h"
#include "PlayMenuWidget.h"
#include "PlayHUDWidget.h"
#include "MonsterHPWidget.h"
#include "SystemMessageWidget.h"
#include "GameEndWidget.h"





ANovaPlayerController::ANovaPlayerController()
{
	static ConstructorHelpers::FClassFinder<ANovaCharacter> PlayerPawnObject(TEXT(
		"/Game/NovaProject/Blueprints/BP_Character.BP_Character_C"));

	if (PlayerPawnObject.Class != NULL)
	{
		WhoToSpawn = PlayerPawnObject.Class->GetDefaultObject<ANovaCharacter>();
	}

	static ConstructorHelpers::FClassFinder<AMonsterCharacter> MonsterPawnObject(TEXT(
		"/Game/NovaProject/Blueprints/BP_Monster_Boss.BP_Monster_Boss_C"));

	if (MonsterPawnObject.Class != NULL)
	{
		MonsterSpawn = MonsterPawnObject.Class->GetDefaultObject<AMonsterCharacter>();
	}

	static ConstructorHelpers::FClassFinder<AMonsterCharacter> MonsterPawnObject2(TEXT(
		"/Game/NovaProject/Blueprints/BP_Monster_Normal.BP_Monster_Normal_C"));

	if (MonsterPawnObject2.Class != NULL)
	{
		Monster_Normal = MonsterPawnObject2.Class->GetDefaultObject<AMonsterCharacter>();
	}

	static ConstructorHelpers::FClassFinder<AMonsterCharacter> MonsterPawnObject3(TEXT(
		"/Game/NovaProject/Blueprints/BP_Monster_Rare.BP_Monster_Rare_C"));

	if (MonsterPawnObject3.Class != NULL)
	{
		Monster_Rare = MonsterPawnObject3.Class->GetDefaultObject<AMonsterCharacter>();
	}

	static ConstructorHelpers::FClassFinder<UPlayMenuWidget> UI_MENU_C(TEXT(
		"/Game/NovaProject/UI/BP_Menu.BP_Menu_C"));
	if (UI_MENU_C.Succeeded())
	{
		MenuWidgetClass = UI_MENU_C.Class->GetDefaultObject<UPlayMenuWidget>();
	}

	// 블루프린트로 제작된 블루프린트 위젯을 연결해준다.
	static ConstructorHelpers::FClassFinder<UPlayHUDWidget> UI_HUD_C(TEXT(
		"/Game/NovaProject/UI/BP_HUD.BP_HUD_C"));
	if (UI_HUD_C.Succeeded())
	{
		PlayHUD = UI_HUD_C.Class->GetDefaultObject<UPlayHUDWidget>();
	}

	// 플레이어가 죽었을때 띄워줄 위젯클래스
	static ConstructorHelpers::FClassFinder<UDiedHUD> UI_DIED_HUD(TEXT(
		"/Game/NovaProject/UI/BP_DiedPlayer.BP_DiedPlayer_C"));
	if (UI_DIED_HUD.Succeeded())
	{
		DiedHUD = UI_DIED_HUD.Class->GetDefaultObject<UDiedHUD>();
	}


	// 게임종료 처리 위젯을 띄워줄 부분
	static ConstructorHelpers::FClassFinder<UGameEndWidget> UI_SYS_HUD(TEXT(
		"/Game/NovaProject/UI/BP_GameEnd.BP_GameEnd_C"));
	if (UI_SYS_HUD.Succeeded())
	{
		GameEndHUD = UI_SYS_HUD.Class->GetDefaultObject<UGameEndWidget>();
	}

	/*static ConstructorHelpers::FClassFinder<UGameEndWidget> UI_SYSWin_HUD(TEXT(
		"/Game/NovaProject/UI/BP_GameWin.BP_GameWin_C"));
	if (UI_SYSWin_HUD.Succeeded())
	{
		GameWinHUD = UI_SYSWin_HUD.Class->GetDefaultObject<UGameEndWidget>();
	}*/


	

	//static ConstructorHelpers::FClassFinder<UMonsterHPWidget> UI_HUD_Mon(TEXT(
	//	"/Game/NovaProject/UI/BP_HPBarMonster.BP_HPBarMonster"));
	//if (UI_HUD_Mon.Succeeded())
	//{
	//	MonsterHPBarWidget->SetWidgetClass(UI_HUD_Mon.Class);
	//	MonsterHPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	//}
	

	gamestart = false;
}

// 최초의 동작
void ANovaPlayerController::BeginPlay()
{
	// Tick을 작동 시킬건지 아닌지
	PrimaryActorTick.bCanEverTick = true;


	// 게임 인풋 모드 설정
	FInputModeGameAndUI GameMode;
	SetInputMode(GameMode);
	bShowMouseCursor = true;
	

	// 타이틀 인스턴스를 만들어서 위젯을 생성해주고 뷰포트에 띄워준다.
	PlayHUDClass = CreateWidget<UPlayHUDWidget>(GetWorld(), PlayHUD->GetClass());
	PlayHUDClass->AddToViewport();
	

	MenuWidget = CreateWidget<UPlayMenuWidget>(GetWorld(), MenuWidgetClass->GetClass());
	ABCHECK(nullptr != MenuWidget);

	// 캐릭터 액터를 가져온다.
	auto Character_cli = Cast<ANovaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Character_cli) return;

	// 캐릭터 이름을 저장해준다.
	Character_cli->CharacterID = UNovaGameInstance::IDcode;

	// 서버에 보낼 캐릭터 데이터 작성
	flatbuffers::FlatBufferBuilder builder(8192); // 플랫버퍼 생성
	auto Location = object::CreateLocation(builder, // 로케이션값 생성
		Character_cli->GetActorLocation().X,
		Character_cli->GetActorLocation().Y,
		Character_cli->GetActorLocation().Z);

	auto Rotation = object::CreateRotation(builder, // 로테이션값 생성
		Character_cli->GetActorRotation().Pitch,
		Character_cli->GetActorRotation().Roll,
		Character_cli->GetActorRotation().Yaw);

	auto Velocity = object::CreateVelocity(builder,
		Character_cli->GetVelocity().X,
		Character_cli->GetVelocity().Y,
		Character_cli->GetVelocity().Z);

	auto Position = object::CreatePosition(builder, // 포지션에 로케이션, 로테이션값 넣어준다.
		Location,
		Rotation,
		Velocity);

	auto pd = object::CreatePlayerData(builder, // 플레이어 데이터 값 작성
		0,
		Character_cli->CharacterID,
		static_cast<int32>(Character_cli->CharacterCurrentHP),
		Position,
		Character_cli->IsPlayerAlive,
		Character_cli->IsAttacking);
	auto pk = object::CreatePacket(builder, object::Type_PlayerData, pd.Union()); // 패킷으로 묶어준다.

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	int32 sent = 0;

	// 데이터를 보낸다.
	bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);
	//builder.Release();

	UE_LOG(LogTemp, Warning, TEXT("SignUp Send Result : [ %d ] (1: true, 0: false)"), nSendLend);
	

	// 0.016초, 즉 1초에 60번 서버에 캐릭터 정보를 전송한다.
	// 0.064초, 즉 1초에 15번 서버에 캐릭터 정보를 전송.
	//if (UNovaGameInstance::MyRoomInfo.user_list_size() > 1) {
		// 싱글게임인 경우엔 주기적인 캐릭터 정보를 보낼 필요가 없다.
	// 싱글게임이어도 서버랑 데이터를 동기화 한다.
		GetWorldTimerManager().SetTimer(SendPlayerInfoHandle, this, &ANovaPlayerController::SendPlayerInfo, 0.016f, true);
	//}
	UNovaGameInstance::MyCharacterData = Character_cli;
	UNovaGameInstance::IsNowGamePlaying = true;

}

void ANovaPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

}

void ANovaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("GamePause"), EInputEvent::IE_Pressed, this, &ANovaPlayerController::OnGamePause);


}

void ANovaPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!UNovaGameInstance::bIsConnected) return;

	// 이 스위치가 동작하는 경우는, 다른 유저의 캐릭터를 스폰시켜주는 경우
	if (UNovaGameInstance::GamePlayerUpdate)
	{
		SpawnNewPlayerChatacter();
	}

	if (UNovaGameInstance::GameMonsterUpdate)
	{
		SpawnMonster();
	}

	if (UNovaGameInstance::GameCounting && UNovaGameInstance::IsNowGamePlaying)
	{
		StartCounting();
	}
	
	//SendPlayerInfo(); // 매 틱단위당 서버에 플레이어 정보 전송
	if (gamestart) 
	{
		UpdateWorld(); // 매 틱단위당 클라이언트의 게임플레이 업데이트
		UpdateHPBar();
	}
	
	if (UNovaGameInstance::IsNeedUpdateMessage)
	{
		UpdateNewMessage();
	}

	if (UNovaGameInstance::UpdateSystemMessage)
	{
		UpdateSystemMessage();
	}

	if (UNovaGameInstance::UpdateTowerHP && UNovaGameInstance::IsNowGamePlaying)
	{
		UpdateTowerHealth();
	}

	if (UNovaGameInstance::UpdateGameTime && UNovaGameInstance::IsNowGamePlaying) {
		UpdateGameTime();
	}
	
}



bool ANovaPlayerController::WriteMessageToChatWindow(const FString& msg)
{
	int32 gamecode = UNovaGameInstance::NowPlayStateCode;
	// FString -> string
	std::string userid = std::string(TCHAR_TO_UTF8(*UNovaGameInstance::GetUserID()));

	// 입력한 채팅 메세지를 서버에 보낸다.
	//bool nSendnend = UNovaGameInstance::NovaSocketcli->SendMessageToServer(msg, gamecode, userid);;

	//return nSendnend;
	return false;
}

void ANovaPlayerController::UpdateNewMessage()
{
	UPlayHUDWidget* lbwg = Cast<UPlayHUDWidget>(PlayHUDClass);
	if (lbwg != nullptr)
	{
		// UTF8 형식으로 문자열을 바꿔줘야 이상한 외계어가 안뜬다.
		FString msg = UTF8_TO_TCHAR(UNovaGameInstance::ReceiveChatMessage.c_str());
		FString id = UTF8_TO_TCHAR(UNovaGameInstance::ReceiveMsgUserID.c_str());
		UE_LOG(LogTemp, Warning, TEXT("UPdateChat"));
		// FString -> FText로 변환하여 전달.
		lbwg->UpdateChatmsg(FText::FromString(id), FText::FromString(msg));
	}
	UNovaGameInstance::IsNeedUpdateMessage = false;
}

void ANovaPlayerController::UpdateSystemMessage()
{
	UPlayHUDWidget* lbwg = Cast<UPlayHUDWidget>(PlayHUDClass);
	if (lbwg != nullptr)
	{
		FString msg = UTF8_TO_TCHAR(UNovaGameInstance::ReceiveSystemMessage.c_str());

		lbwg->UpdateSysmsg(FText::FromString(msg), UNovaGameInstance::SystemMessageCode);
	}
	UNovaGameInstance::UpdateSystemMessage = false;
}

void ANovaPlayerController::OnGamePause()
{
	
	MenuWidget->AddToViewport(3);

	/*FInputModeUIOnly GameMode;
	SetInputMode(GameMode);
	bShowMouseCursor = true;*/

	SetPause(true);
	//ChangeInputMode(false);
}

// 처음접속한 캐릭터를 스폰시켜주는 부분이다.
void ANovaPlayerController::SpawnNewPlayerChatacter()
{
	if (UNovaGameInstance::TempPlayerData.player_name != -1) {
		PlayerDataClass materialInfo = UNovaGameInstance::TempPlayerData;

		// 클라이언트 자신의 정보라면 필요없으므로 지운다.
		if (materialInfo.player_name == UNovaGameInstance::IDcode) {
			UNovaGameInstance::GamePlayerUpdate = false;
			//UNovaGameInstance::TempPlayerData.Clear();
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("SpawnNewCharacter : %d"), materialInfo.player_name);

		// 받아온 위치정보를 세팅해준다.
		FVector SpawnLocation;
		SpawnLocation.X = materialInfo.Loc_X;
		SpawnLocation.Y = materialInfo.Loc_Y;
		SpawnLocation.Z = materialInfo.Loc_Z;

		// 받아온 회전정보를 세팅해준다.
		FRotator SpawnRotation;
		SpawnRotation.Yaw = materialInfo.Rot_Yaw;
		SpawnRotation.Pitch = materialInfo.Rot_Pitch;
		SpawnRotation.Roll = materialInfo.Rot_Roll;

		// ???
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		// 새 캐릭터를 생성해서 월드에 스폰시켜준다.
		auto SpawnCharacter = GetWorld()->SpawnActor<ANovaCharacter>(WhoToSpawn->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
		// 기본적인 컨트롤러를 연결시켜준다.
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->CharacterID = materialInfo.player_name;
		SpawnCharacter->CharacterName = materialInfo.CharacterName;

		// 그리고 접속한 캐릭터 정보를 다룰 배열에 저장해준다.
		UNovaGameInstance::PlayingGameUserList.Add(SpawnCharacter->CharacterID, SpawnCharacter);
		GLog->Log(FString::Printf(TEXT("First Player Data! Num : %d"), UNovaGameInstance::PlayingGameUserList.Num()) + " | ANovaPlayerController::SpawnNewPlayerChatacter");

		// 받아온 데이터는 지운다.
		UNovaGameInstance::TempPlayerData.player_name = -1;
	}
	// 한번만 작동해야 하므로, 캐릭터를 스폰시킨 뒤엔 스위치를 꺼준다.
	UNovaGameInstance::GamePlayerUpdate = false;

}

void ANovaPlayerController::SpawnMonster()
{
	if (UNovaGameInstance::TempMonsterData.player_name != 0) {
		MonsterDataClass materialInfo = UNovaGameInstance::TempMonsterData;

		ABLOG(Error, TEXT("SpawnMonster ID : %d"), materialInfo.player_name);


		// 받아온 위치정보를 세팅해준다.
		FVector SpawnLocation;
		SpawnLocation.X = materialInfo.Loc_X;
		SpawnLocation.Y = materialInfo.Loc_Y;
		SpawnLocation.Z = materialInfo.Loc_Z;

		// 받아온 회전정보를 세팅해준다.
		FRotator SpawnRotation;
		SpawnRotation.Yaw = 0.0f;
		SpawnRotation.Pitch = 0.0f;
		SpawnRotation.Roll = 0.0f;

		// ???
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ABLOG(Error, TEXT(" Monster Type : %d"), materialInfo.MonsterType);


		// 새 캐릭터를 생성해서 월드에 스폰시켜준다.
		if (materialInfo.MonsterType == 1)
		{ // 일반몬스터
			ABLOG(Error, TEXT(" Normal Monster"));
			auto SpawnedMonster = GetWorld()->SpawnActor<AMonsterCharacter>(Monster_Normal->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
			// 기본적인 컨트롤러를 연결시켜준다.
			SpawnedMonster->SpawnDefaultController();
			SpawnedMonster->Monsterid = materialInfo.player_name;
			SpawnedMonster->MonsterType = materialInfo.MonsterType;
			SpawnedMonster->MonsterMaxHP = static_cast<float>(materialInfo.health);
			SpawnedMonster->MonsterCurrentHP = static_cast<float>( materialInfo.health);

			// 그리고 접속한 캐릭터 정보를 다룰 배열에 저장해준다.
			UNovaGameInstance::MonsterList.Add(SpawnedMonster->Monsterid, SpawnedMonster);

		}
		else if (materialInfo.MonsterType == 2) // 레어몬스터
		{
			ABLOG(Error, TEXT(" Rare Monster"));

			auto SpawnedMonster = GetWorld()->SpawnActor<AMonsterCharacter>(Monster_Rare->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
			// 기본적인 컨트롤러를 연결시켜준다.
			SpawnedMonster->SpawnDefaultController();
			SpawnedMonster->Monsterid = materialInfo.player_name;
			SpawnedMonster->MonsterType = materialInfo.MonsterType;
			SpawnedMonster->MonsterMaxHP = static_cast<float>(materialInfo.health);
			SpawnedMonster->MonsterCurrentHP = static_cast<float>(materialInfo.health);

			// 그리고 접속한 캐릭터 정보를 다룰 배열에 저장해준다.
			UNovaGameInstance::MonsterList.Add(SpawnedMonster->Monsterid, SpawnedMonster);

		}
		else if (materialInfo.MonsterType == 3) // 보스몬스터
		{
			ABLOG(Error, TEXT(" Boss Monster"));

			auto SpawnedMonster = GetWorld()->SpawnActor<AMonsterCharacter>(MonsterSpawn->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
			// 기본적인 컨트롤러를 연결시켜준다.
			SpawnedMonster->SpawnDefaultController();
			SpawnedMonster->Monsterid = materialInfo.player_name;
			SpawnedMonster->MonsterType = materialInfo.MonsterType;
			SpawnedMonster->MonsterMaxHP = static_cast<float>(materialInfo.health);
			SpawnedMonster->MonsterCurrentHP = static_cast<float>(materialInfo.health);

			// 그리고 접속한 캐릭터 정보를 다룰 배열에 저장해준다.
			UNovaGameInstance::MonsterList.Add(SpawnedMonster->Monsterid, SpawnedMonster);

		}


		GLog->Log(FString::Printf(TEXT("Current Monster! Num : %d"), UNovaGameInstance::MonsterList.Num()) + " | ANovaPlayerController::SpawnMonster");




		// 받아온 데이터는 지운다.
		UNovaGameInstance::TempMonsterData.player_name = 0;
		UNovaGameInstance::NovaSocketcli->SendSpawnOK(); // 스폰되었다고 서버에 보내준다.
		// 한번만 작동해야 하므로, 캐릭터를 스폰시킨 뒤엔 스위치를 꺼준다.
	}
	UNovaGameInstance::GameMonsterUpdate = false;
}

void ANovaPlayerController::SendPlayerInfo()
{
	auto Character_cli = UNovaGameInstance::MyCharacterData;
	if (!Character_cli) return;

	// 캐릭터 이름을 저장해준다.
	Character_cli->CharacterID = UNovaGameInstance::IDcode;

	// 서버에 보낼 캐릭터 데이터 작성
	flatbuffers::FlatBufferBuilder builder(8192); // 플랫버퍼 생성
	auto Location = object::CreateLocation(builder, // 로케이션값 생성
		Character_cli->GetActorLocation().X,
		Character_cli->GetActorLocation().Y,
		Character_cli->GetActorLocation().Z);

	auto Rotation = object::CreateRotation(builder, // 로테이션값 생성
		Character_cli->GetActorRotation().Pitch,
		Character_cli->GetActorRotation().Roll,
		Character_cli->GetActorRotation().Yaw);

	auto Velocity = object::CreateVelocity(builder,
		Character_cli->GetVelocity().X,
		Character_cli->GetVelocity().Y,
		Character_cli->GetVelocity().Z);

	auto Position = object::CreatePosition(builder, // 포지션에 로케이션, 로테이션값 넣어준다.
		Location,
		Rotation,
		Velocity);

	auto pd = object::CreatePlayerData(builder, // 플레이어 데이터 값 작성
		builder.CreateString(UNovaGameInstance::userLoginID),
		Character_cli->CharacterID,
		static_cast<int32>(Character_cli->CharacterCurrentHP),
		Position,
		Character_cli->IsPlayerAlive,
		Character_cli->IsAttacking,
		Character_cli->AttackingCountNumber,
		Character_cli->IsSkillUsing,
		Character_cli->SkillUsingNumber);
	auto pk = object::CreatePacket(builder, object::Type_PlayerData, pd.Union()); // 패킷으로 묶어준다.

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	int32 sent = 0;

	// 데이터를 보낸다.
	bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);

}

bool ANovaPlayerController::UpdateWorld()
{
	if (GetWorld() == nullptr) return false;



	//만약 캐릭터가 살아있는 상태인데, 체력이 0이되는 경우, 죽는 모션을 취하게 하고 상태를 죽은것으로 바꿔준다.
	//ABLOG(Error, TEXT("MyHP !!! : %f"), UNovaGameInstance::MyCharacterData->CharacterCurrentHP);
	if (UNovaGameInstance::MyCharacterData->IsPlayerAlive == true && UNovaGameInstance::MyCharacterData->CharacterCurrentHP < 0.0f) {
		ABLOG(Error, TEXT("YOU DIED!!!"));
		UNovaGameInstance::MyCharacterData->IsPlayerAlive = false;
		UNovaGameInstance::MyCharacterData->CharacterDead();

		DiedHUDClass = CreateWidget<UDiedHUD>(GetWorld(), DiedHUD->GetClass());
		DiedHUDClass->AddToViewport(5);
		DiedHUDClass->StartCounting();

		//죽은횟수증가
		UNovaGameInstance::DiedValue++;

		FInputModeUIOnly GameMode;
		SetInputMode(GameMode);
		bShowMouseCursor = true;

	}

	if (UNovaGameInstance::MyRoomInfo->IsGameEnd && !UNovaGameInstance::MyRoomInfo->EndWidget) // 게임엔드 처리가 된 경우, 게임끝 위젯을 띄운다.
	{
		UNovaGameInstance::MyRoomInfo->EndWidget = true;
		GameEndClass = CreateWidget<UGameEndWidget>(GetWorld(), GameEndHUD->GetClass());
		//ABCHECK(nullptr != GameEndClass);
		if (UNovaGameInstance::MyRoomInfo->IsGameWin) {
			GameEndClass->SetGameOverOrGameWin(1);
			ABLOG(Warning, TEXT("GameWin!!!"));
		}
		else {		
			GameEndClass->SetGameOverOrGameWin(0);
			ABLOG(Warning, TEXT("GameLose!!!"));
		}
		GameEndClass->AddToViewport(5);
		SetPause(true);
		UNovaGameInstance::IsNowGamePlaying = false;

		//그리고 서버에 게임종료했다는 메세지를 보내준다.
		flatbuffers::FlatBufferBuilder builder(1024); // 데이터를 담을 플랫버퍼빌더를 만든다.
		auto msg = object::CreateGameEnd(builder,
			true);
		auto pk = object::CreatePacket(builder, object::Type_GameEnd, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

		// 나머지 부분은 공통 손대지말자
		builder.Finish(pk);
		uint8_t* send_buffer = builder.GetBufferPointer();
		int32 sent = 0;

		// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
		bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);
		UE_LOG(LogTemp, Warning, TEXT("Send Game End 1"))

		//// 남은시간이 0이되어 끝났으므로, 게임 승리
		//// 끝날때의 남은 타워의 체력을 저장해준다..
		//UNovaGameInstance::GameEndLeftTime = 180;
		//UNovaGameInstance::GameEndLeftTowerHP = UNovaGameInstance::TowerHP;
		//ABLOG(Warning, TEXT("GameWin!!!"));
	}

	
	
	return true;
}

void ANovaPlayerController::StartCounting()
{
	UPlayHUDWidget* lbwg = Cast<UPlayHUDWidget>(PlayHUDClass);
	if (lbwg != nullptr)
	{
		lbwg->PlayStartCounting();
		lbwg->WaitingPanelSetBlur();
	}
	if (UNovaGameInstance::CountingNumber == 0) {
		lbwg->RemoveWaitingPanel();
		gamestart = true;
		//UNovaGameInstance::NovaSocketcli->SendGameStartInfo(UNovaGameInstance::NowPlayStateCode, true);
 
		lbwg->SetPlayersHUD();

		auto GI = Cast<UNovaGameInstance>(GetGameInstance());
		GI->StartGarbegeSwaper();
	}

	UNovaGameInstance::GameCounting = false;
}

void ANovaPlayerController::UpdateGameTime()
{
	if (UNovaGameInstance::GameTime >= 0) {
		UPlayHUDWidget* lbwg = Cast<UPlayHUDWidget>(PlayHUDClass);
		if (lbwg != nullptr)
		{
			lbwg->SetGameTime(UNovaGameInstance::GameTime);
		}
	}
	//else if(UNovaGameInstance::GameTime < 0)
	//{
	//	GameEndClass = CreateWidget<UGameEndWidget>(GetWorld(), GameEndHUD->GetClass());
	//	ABCHECK(nullptr != GameEndClass);
	//	GameEndClass->SetGameOverOrGameWin(1);
	//	GameEndClass->AddToViewport(5);
	//	SetPause(true);
	//	UNovaGameInstance::IsNowGamePlaying = false;

	//	// 남은시간이 0이되어 끝났으므로, 게임 승리
	//	// 끝날때의 남은 타워의 체력을 저장해준다..
	//	UNovaGameInstance::GameEndLeftTime = 180;
	//	UNovaGameInstance::GameEndLeftTowerHP = UNovaGameInstance::TowerHP;
	//	ABLOG(Warning, TEXT("GameWin!!!"));
	//}

	UNovaGameInstance::UpdateGameTime = false;
}

void ANovaPlayerController::Skill1_Using_HUD_Count()
{
	UPlayHUDWidget* lbwg = Cast<UPlayHUDWidget>(PlayHUDClass);
	if (lbwg != nullptr)
	{
		lbwg->StartSkill1_Count();
	}
}

void ANovaPlayerController::Skill2_Using_HUD_Count()
{
	UPlayHUDWidget* lbwg = Cast<UPlayHUDWidget>(PlayHUDClass);
	if (lbwg != nullptr)
	{
		lbwg->StartSkill2_Count();
	}
}

void ANovaPlayerController::UpdateHPBar()
{
	UPlayHUDWidget* lbwg = Cast<UPlayHUDWidget>(PlayHUDClass);
	if (lbwg != nullptr)
	{
		//ABLOG(Error, TEXT("Update HP BAR!!!"));
		lbwg->UpdateHPbar(Cast<ANovaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)));
		lbwg->UpdateCurrentPoint();
		lbwg->UpdatePotion();
	} 
}

void ANovaPlayerController::UpdateTowerHealth()
{
	if (UNovaGameInstance::TowerHP > 0) {
		UPlayHUDWidget* lbwg = Cast<UPlayHUDWidget>(PlayHUDClass);
		if (lbwg != nullptr)
		{
			//ABLOG(Error, TEXT("Update HP BAR!!!"));

			lbwg->SetTowerHP(UNovaGameInstance::TowerHP);
		}
	}
	else if (UNovaGameInstance::TowerHP <= 0)
	{
		//GameEndClass = CreateWidget<UGameEndWidget>(GetWorld(), GameEndHUD->GetClass());
		//ABCHECK(nullptr != GameEndClass);
		//GameEndClass->SetGameOverOrGameWin(0);
		//GameEndClass->AddToViewport(5);
		//SetPause(true);
		//UNovaGameInstance::IsNowGamePlaying = false;

		//// 타워의 체력이 0이되어 게임이 끝났으므로
		//// 끝날때의 남은 시간을 체크해서 저장해준다.
		//UNovaGameInstance::GameEndLeftTime = 180 - UNovaGameInstance::GameTime;
		//UNovaGameInstance::GameEndLeftTowerHP = 0;
		//ABLOG(Warning, TEXT("GameLose!!!"));


	}
	

	UNovaGameInstance::UpdateTowerHP = false;
}


