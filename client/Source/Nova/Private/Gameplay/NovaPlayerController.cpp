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

	// �������Ʈ�� ���۵� �������Ʈ ������ �������ش�.
	static ConstructorHelpers::FClassFinder<UPlayHUDWidget> UI_HUD_C(TEXT(
		"/Game/NovaProject/UI/BP_HUD.BP_HUD_C"));
	if (UI_HUD_C.Succeeded())
	{
		PlayHUD = UI_HUD_C.Class->GetDefaultObject<UPlayHUDWidget>();
	}

	// �÷��̾ �׾����� ����� ����Ŭ����
	static ConstructorHelpers::FClassFinder<UDiedHUD> UI_DIED_HUD(TEXT(
		"/Game/NovaProject/UI/BP_DiedPlayer.BP_DiedPlayer_C"));
	if (UI_DIED_HUD.Succeeded())
	{
		DiedHUD = UI_DIED_HUD.Class->GetDefaultObject<UDiedHUD>();
	}


	// �������� ó�� ������ ����� �κ�
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

// ������ ����
void ANovaPlayerController::BeginPlay()
{
	// Tick�� �۵� ��ų���� �ƴ���
	PrimaryActorTick.bCanEverTick = true;


	// ���� ��ǲ ��� ����
	FInputModeGameAndUI GameMode;
	SetInputMode(GameMode);
	bShowMouseCursor = true;
	

	// Ÿ��Ʋ �ν��Ͻ��� ���� ������ �������ְ� ����Ʈ�� ����ش�.
	PlayHUDClass = CreateWidget<UPlayHUDWidget>(GetWorld(), PlayHUD->GetClass());
	PlayHUDClass->AddToViewport();
	

	MenuWidget = CreateWidget<UPlayMenuWidget>(GetWorld(), MenuWidgetClass->GetClass());
	ABCHECK(nullptr != MenuWidget);

	// ĳ���� ���͸� �����´�.
	auto Character_cli = Cast<ANovaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Character_cli) return;

	// ĳ���� �̸��� �������ش�.
	Character_cli->CharacterID = UNovaGameInstance::IDcode;

	// ������ ���� ĳ���� ������ �ۼ�
	flatbuffers::FlatBufferBuilder builder(8192); // �÷����� ����
	auto Location = object::CreateLocation(builder, // �����̼ǰ� ����
		Character_cli->GetActorLocation().X,
		Character_cli->GetActorLocation().Y,
		Character_cli->GetActorLocation().Z);

	auto Rotation = object::CreateRotation(builder, // �����̼ǰ� ����
		Character_cli->GetActorRotation().Pitch,
		Character_cli->GetActorRotation().Roll,
		Character_cli->GetActorRotation().Yaw);

	auto Velocity = object::CreateVelocity(builder,
		Character_cli->GetVelocity().X,
		Character_cli->GetVelocity().Y,
		Character_cli->GetVelocity().Z);

	auto Position = object::CreatePosition(builder, // �����ǿ� �����̼�, �����̼ǰ� �־��ش�.
		Location,
		Rotation,
		Velocity);

	auto pd = object::CreatePlayerData(builder, // �÷��̾� ������ �� �ۼ�
		0,
		Character_cli->CharacterID,
		static_cast<int32>(Character_cli->CharacterCurrentHP),
		Position,
		Character_cli->IsPlayerAlive,
		Character_cli->IsAttacking);
	auto pk = object::CreatePacket(builder, object::Type_PlayerData, pd.Union()); // ��Ŷ���� �����ش�.

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	int32 sent = 0;

	// �����͸� ������.
	bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);
	//builder.Release();

	UE_LOG(LogTemp, Warning, TEXT("SignUp Send Result : [ %d ] (1: true, 0: false)"), nSendLend);
	

	// 0.016��, �� 1�ʿ� 60�� ������ ĳ���� ������ �����Ѵ�.
	// 0.064��, �� 1�ʿ� 15�� ������ ĳ���� ������ ����.
	//if (UNovaGameInstance::MyRoomInfo.user_list_size() > 1) {
		// �̱۰����� ��쿣 �ֱ����� ĳ���� ������ ���� �ʿ䰡 ����.
	// �̱۰����̾ ������ �����͸� ����ȭ �Ѵ�.
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

	// �� ����ġ�� �����ϴ� ����, �ٸ� ������ ĳ���͸� ���������ִ� ���
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
	
	//SendPlayerInfo(); // �� ƽ������ ������ �÷��̾� ���� ����
	if (gamestart) 
	{
		UpdateWorld(); // �� ƽ������ Ŭ���̾�Ʈ�� �����÷��� ������Ʈ
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

	// �Է��� ä�� �޼����� ������ ������.
	//bool nSendnend = UNovaGameInstance::NovaSocketcli->SendMessageToServer(msg, gamecode, userid);;

	//return nSendnend;
	return false;
}

void ANovaPlayerController::UpdateNewMessage()
{
	UPlayHUDWidget* lbwg = Cast<UPlayHUDWidget>(PlayHUDClass);
	if (lbwg != nullptr)
	{
		// UTF8 �������� ���ڿ��� �ٲ���� �̻��� �ܰ� �ȶ��.
		FString msg = UTF8_TO_TCHAR(UNovaGameInstance::ReceiveChatMessage.c_str());
		FString id = UTF8_TO_TCHAR(UNovaGameInstance::ReceiveMsgUserID.c_str());
		UE_LOG(LogTemp, Warning, TEXT("UPdateChat"));
		// FString -> FText�� ��ȯ�Ͽ� ����.
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

// ó�������� ĳ���͸� ���������ִ� �κ��̴�.
void ANovaPlayerController::SpawnNewPlayerChatacter()
{
	if (UNovaGameInstance::TempPlayerData.player_name != -1) {
		PlayerDataClass materialInfo = UNovaGameInstance::TempPlayerData;

		// Ŭ���̾�Ʈ �ڽ��� ������� �ʿ�����Ƿ� �����.
		if (materialInfo.player_name == UNovaGameInstance::IDcode) {
			UNovaGameInstance::GamePlayerUpdate = false;
			//UNovaGameInstance::TempPlayerData.Clear();
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("SpawnNewCharacter : %d"), materialInfo.player_name);

		// �޾ƿ� ��ġ������ �������ش�.
		FVector SpawnLocation;
		SpawnLocation.X = materialInfo.Loc_X;
		SpawnLocation.Y = materialInfo.Loc_Y;
		SpawnLocation.Z = materialInfo.Loc_Z;

		// �޾ƿ� ȸ�������� �������ش�.
		FRotator SpawnRotation;
		SpawnRotation.Yaw = materialInfo.Rot_Yaw;
		SpawnRotation.Pitch = materialInfo.Rot_Pitch;
		SpawnRotation.Roll = materialInfo.Rot_Roll;

		// ???
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		// �� ĳ���͸� �����ؼ� ���忡 ���������ش�.
		auto SpawnCharacter = GetWorld()->SpawnActor<ANovaCharacter>(WhoToSpawn->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
		// �⺻���� ��Ʈ�ѷ��� ��������ش�.
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->CharacterID = materialInfo.player_name;
		SpawnCharacter->CharacterName = materialInfo.CharacterName;

		// �׸��� ������ ĳ���� ������ �ٷ� �迭�� �������ش�.
		UNovaGameInstance::PlayingGameUserList.Add(SpawnCharacter->CharacterID, SpawnCharacter);
		GLog->Log(FString::Printf(TEXT("First Player Data! Num : %d"), UNovaGameInstance::PlayingGameUserList.Num()) + " | ANovaPlayerController::SpawnNewPlayerChatacter");

		// �޾ƿ� �����ʹ� �����.
		UNovaGameInstance::TempPlayerData.player_name = -1;
	}
	// �ѹ��� �۵��ؾ� �ϹǷ�, ĳ���͸� ������Ų �ڿ� ����ġ�� ���ش�.
	UNovaGameInstance::GamePlayerUpdate = false;

}

void ANovaPlayerController::SpawnMonster()
{
	if (UNovaGameInstance::TempMonsterData.player_name != 0) {
		MonsterDataClass materialInfo = UNovaGameInstance::TempMonsterData;

		ABLOG(Error, TEXT("SpawnMonster ID : %d"), materialInfo.player_name);


		// �޾ƿ� ��ġ������ �������ش�.
		FVector SpawnLocation;
		SpawnLocation.X = materialInfo.Loc_X;
		SpawnLocation.Y = materialInfo.Loc_Y;
		SpawnLocation.Z = materialInfo.Loc_Z;

		// �޾ƿ� ȸ�������� �������ش�.
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


		// �� ĳ���͸� �����ؼ� ���忡 ���������ش�.
		if (materialInfo.MonsterType == 1)
		{ // �Ϲݸ���
			ABLOG(Error, TEXT(" Normal Monster"));
			auto SpawnedMonster = GetWorld()->SpawnActor<AMonsterCharacter>(Monster_Normal->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
			// �⺻���� ��Ʈ�ѷ��� ��������ش�.
			SpawnedMonster->SpawnDefaultController();
			SpawnedMonster->Monsterid = materialInfo.player_name;
			SpawnedMonster->MonsterType = materialInfo.MonsterType;
			SpawnedMonster->MonsterMaxHP = static_cast<float>(materialInfo.health);
			SpawnedMonster->MonsterCurrentHP = static_cast<float>( materialInfo.health);

			// �׸��� ������ ĳ���� ������ �ٷ� �迭�� �������ش�.
			UNovaGameInstance::MonsterList.Add(SpawnedMonster->Monsterid, SpawnedMonster);

		}
		else if (materialInfo.MonsterType == 2) // �������
		{
			ABLOG(Error, TEXT(" Rare Monster"));

			auto SpawnedMonster = GetWorld()->SpawnActor<AMonsterCharacter>(Monster_Rare->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
			// �⺻���� ��Ʈ�ѷ��� ��������ش�.
			SpawnedMonster->SpawnDefaultController();
			SpawnedMonster->Monsterid = materialInfo.player_name;
			SpawnedMonster->MonsterType = materialInfo.MonsterType;
			SpawnedMonster->MonsterMaxHP = static_cast<float>(materialInfo.health);
			SpawnedMonster->MonsterCurrentHP = static_cast<float>(materialInfo.health);

			// �׸��� ������ ĳ���� ������ �ٷ� �迭�� �������ش�.
			UNovaGameInstance::MonsterList.Add(SpawnedMonster->Monsterid, SpawnedMonster);

		}
		else if (materialInfo.MonsterType == 3) // ��������
		{
			ABLOG(Error, TEXT(" Boss Monster"));

			auto SpawnedMonster = GetWorld()->SpawnActor<AMonsterCharacter>(MonsterSpawn->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
			// �⺻���� ��Ʈ�ѷ��� ��������ش�.
			SpawnedMonster->SpawnDefaultController();
			SpawnedMonster->Monsterid = materialInfo.player_name;
			SpawnedMonster->MonsterType = materialInfo.MonsterType;
			SpawnedMonster->MonsterMaxHP = static_cast<float>(materialInfo.health);
			SpawnedMonster->MonsterCurrentHP = static_cast<float>(materialInfo.health);

			// �׸��� ������ ĳ���� ������ �ٷ� �迭�� �������ش�.
			UNovaGameInstance::MonsterList.Add(SpawnedMonster->Monsterid, SpawnedMonster);

		}


		GLog->Log(FString::Printf(TEXT("Current Monster! Num : %d"), UNovaGameInstance::MonsterList.Num()) + " | ANovaPlayerController::SpawnMonster");




		// �޾ƿ� �����ʹ� �����.
		UNovaGameInstance::TempMonsterData.player_name = 0;
		UNovaGameInstance::NovaSocketcli->SendSpawnOK(); // �����Ǿ��ٰ� ������ �����ش�.
		// �ѹ��� �۵��ؾ� �ϹǷ�, ĳ���͸� ������Ų �ڿ� ����ġ�� ���ش�.
	}
	UNovaGameInstance::GameMonsterUpdate = false;
}

void ANovaPlayerController::SendPlayerInfo()
{
	auto Character_cli = UNovaGameInstance::MyCharacterData;
	if (!Character_cli) return;

	// ĳ���� �̸��� �������ش�.
	Character_cli->CharacterID = UNovaGameInstance::IDcode;

	// ������ ���� ĳ���� ������ �ۼ�
	flatbuffers::FlatBufferBuilder builder(8192); // �÷����� ����
	auto Location = object::CreateLocation(builder, // �����̼ǰ� ����
		Character_cli->GetActorLocation().X,
		Character_cli->GetActorLocation().Y,
		Character_cli->GetActorLocation().Z);

	auto Rotation = object::CreateRotation(builder, // �����̼ǰ� ����
		Character_cli->GetActorRotation().Pitch,
		Character_cli->GetActorRotation().Roll,
		Character_cli->GetActorRotation().Yaw);

	auto Velocity = object::CreateVelocity(builder,
		Character_cli->GetVelocity().X,
		Character_cli->GetVelocity().Y,
		Character_cli->GetVelocity().Z);

	auto Position = object::CreatePosition(builder, // �����ǿ� �����̼�, �����̼ǰ� �־��ش�.
		Location,
		Rotation,
		Velocity);

	auto pd = object::CreatePlayerData(builder, // �÷��̾� ������ �� �ۼ�
		builder.CreateString(UNovaGameInstance::userLoginID),
		Character_cli->CharacterID,
		static_cast<int32>(Character_cli->CharacterCurrentHP),
		Position,
		Character_cli->IsPlayerAlive,
		Character_cli->IsAttacking,
		Character_cli->AttackingCountNumber,
		Character_cli->IsSkillUsing,
		Character_cli->SkillUsingNumber);
	auto pk = object::CreatePacket(builder, object::Type_PlayerData, pd.Union()); // ��Ŷ���� �����ش�.

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	int32 sent = 0;

	// �����͸� ������.
	bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);

}

bool ANovaPlayerController::UpdateWorld()
{
	if (GetWorld() == nullptr) return false;



	//���� ĳ���Ͱ� ����ִ� �����ε�, ü���� 0�̵Ǵ� ���, �״� ����� ���ϰ� �ϰ� ���¸� ���������� �ٲ��ش�.
	//ABLOG(Error, TEXT("MyHP !!! : %f"), UNovaGameInstance::MyCharacterData->CharacterCurrentHP);
	if (UNovaGameInstance::MyCharacterData->IsPlayerAlive == true && UNovaGameInstance::MyCharacterData->CharacterCurrentHP < 0.0f) {
		ABLOG(Error, TEXT("YOU DIED!!!"));
		UNovaGameInstance::MyCharacterData->IsPlayerAlive = false;
		UNovaGameInstance::MyCharacterData->CharacterDead();

		DiedHUDClass = CreateWidget<UDiedHUD>(GetWorld(), DiedHUD->GetClass());
		DiedHUDClass->AddToViewport(5);
		DiedHUDClass->StartCounting();

		//����Ƚ������
		UNovaGameInstance::DiedValue++;

		FInputModeUIOnly GameMode;
		SetInputMode(GameMode);
		bShowMouseCursor = true;

	}

	if (UNovaGameInstance::MyRoomInfo->IsGameEnd && !UNovaGameInstance::MyRoomInfo->EndWidget) // ���ӿ��� ó���� �� ���, ���ӳ� ������ ����.
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

		//�׸��� ������ ���������ߴٴ� �޼����� �����ش�.
		flatbuffers::FlatBufferBuilder builder(1024); // �����͸� ���� �÷����ۺ����� �����.
		auto msg = object::CreateGameEnd(builder,
			true);
		auto pk = object::CreatePacket(builder, object::Type_GameEnd, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

		// ������ �κ��� ���� �մ�������
		builder.Finish(pk);
		uint8_t* send_buffer = builder.GetBufferPointer();
		int32 sent = 0;

		// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
		bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);
		UE_LOG(LogTemp, Warning, TEXT("Send Game End 1"))

		//// �����ð��� 0�̵Ǿ� �������Ƿ�, ���� �¸�
		//// �������� ���� Ÿ���� ü���� �������ش�..
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

	//	// �����ð��� 0�̵Ǿ� �������Ƿ�, ���� �¸�
	//	// �������� ���� Ÿ���� ü���� �������ش�..
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

		//// Ÿ���� ü���� 0�̵Ǿ� ������ �������Ƿ�
		//// �������� ���� �ð��� üũ�ؼ� �������ش�.
		//UNovaGameInstance::GameEndLeftTime = 180 - UNovaGameInstance::GameTime;
		//UNovaGameInstance::GameEndLeftTowerHP = 0;
		//ABLOG(Warning, TEXT("GameLose!!!"));


	}
	

	UNovaGameInstance::UpdateTowerHP = false;
}


