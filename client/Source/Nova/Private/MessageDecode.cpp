// Fill out your copyright notice in the Description page of Project Settings.

#include "MessageDecode.h"
#include "NovaGameInstance.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "SystemMessageWidget.h"
#include "Engine/World.h"


void UMessageDecode::ObjectPacketDecoding(TArray<uint8> Buffer)
{
	auto objpacket = object::GetPacket(Buffer.GetData());
	auto MsgType = objpacket->packet_type_type();

	switch (MsgType)
	{

		//===============================================================================================
		//===============================================================================================
		//===============================================================================================

	case object::Type_GameState: //===============================================================================================
	{
		auto state = static_cast<const object::GameState*>(objpacket->packet_type());



			int32 tempcode = state->Code();
			std::string anotherUserid2 = state->UserID()->c_str();
			//UE_LOG(LogTemp, Warning, TEXT("Recevie Code : %d / %s"), tempcode, UTF8_TO_TCHAR(anotherUserid2.c_str()));
			//		// �ٸ� ������ �κ� ���� ���
			if (state->Code() == 0 // �κ� 0
				&& anotherUserid2 != "")
			{
				std::string anotherUserid = state->UserID()->c_str();
				int32 antIDcode = state->UserIDCode();
				int32 gamecode = state->Code();
				int32 MP = state->MaxPoint();

				GLog->Log(FString::Printf(TEXT("DataReceive = LOBBY state / %s"), UTF8_TO_TCHAR(anotherUserid.c_str())));

				//AnotherPlayerInfo temp = AnotherPlayerInfo(anotherUserid, gamecode);
				PlayerInfo* temp = new PlayerInfo();
				temp->Userid = anotherUserid;
				temp->IDcode = antIDcode;
				temp->NowGameStateCode = gamecode;
				temp->MaxPoint = MP;
				UE_LOG(LogTemp, Warning, TEXT("Receive UserList : %s"), UTF8_TO_TCHAR(temp->Userid.c_str()));

				// �κ� ������ ���� 2������ ����, ���ʷ� �α��� �ϴ� ����
				bool isfisrtconnect = true; // �ٸ� ���¿� �ִٰ� �κ�� ���ƿ��� ���

				// ���ʷ� �α����ϴ� ����, ������ ������ ������ ������ ���� ������ �߰������ �ϰ�,
				// �ٸ� ���¿� �ִٰ� �κ�� ���ƿ��� ���� ���������� ���游 ���ָ� �ȴ�.

				// �켱 ������ �ִ� �÷��̾� ������ Ž���� �� ���̵� �ִ��� ã�ƺ���.
				for (int i = 0; i < UNovaGameInstance::PlayerList.Num(); i++) {
					if (UNovaGameInstance::PlayerList[i]->IDcode == antIDcode) {
						// ���� ���̵� ������ �ش� ���̵��� �����ڵ带 �������ְ�
						//UNovaGameInstance::PlayerList[i]->NowGameStateCode = gamecode;
						UNovaGameInstance::PlayerList[i]->NowGameStateCode = gamecode;
						isfisrtconnect = false;
						break;
					}
				}
				// ���ʷ� �α��� �ϴ� ��쿣, �÷��̾� ����Ʈ�� �߰����ش�.
				if (isfisrtconnect == true) {
					UE_LOG(LogTemp, Warning, TEXT("Add Lobby User List : %s"), UTF8_TO_TCHAR(temp->Userid.c_str()));
					UNovaGameInstance::PlayerList.Add(temp);
				}

				// �κ񸮽�Ʈ ������Ʈ
				UNovaGameInstance::LobbyUserListUpdate = true;

				// �÷������� ������ �κ�� ���� ������, ���ӿ��� �ش� ������ ĳ���͸� �ı������ִ� �κ�
				// �� ���� ���� �ٸ� ���¿� �ִٰ� �κ� ���� ����̹Ƿ�, ���¸� �ٽ� ���Ѵ�.
				if (isfisrtconnect == false && UNovaGameInstance::IsNowGamePlaying) {
					// anotherUserid�� �κ�� ���Դٴ� ������ �޾Ҵµ�, ���� Ŭ���̾�Ʈ�� ����Ǿ��ִ� �÷������� ���� ����Ʈ��
					// �ش� ���̵� �ִٸ� �� ���̵�� ��������Ѵ�. ĳ���� ����
					if (UNovaGameInstance::PlayingGameUserList.Contains(antIDcode))
					{
						UNovaGameInstance::PlayingGameUserList[antIDcode]->Destroy();
						UNovaGameInstance::PlayingGameUserList.Remove(antIDcode);

					}

				}
			}



			//
			// �����÷��̸� �ϴ� ���
			else if (state->Code() >= 100)
			{

				std::string anotherUserid = state->UserID()->c_str();
				int32 gamecode = state->Code();
				int32 antIDcode = state->UserIDCode();

				//GLog->Log(FString::Printf(TEXT("DataReceive = GAMEPLAY state / %s"), UTF8_TO_TCHAR(anotherUserid.c_str())));

				if (UNovaGameInstance::NowPlayStateCode == 0) // �κ� 0
				{
					//AnotherPlayerInfo temp = AnotherPlayerInfo(anotherUserid, gamecode);
					PlayerInfo* temp = new PlayerInfo();
					temp->Userid = anotherUserid;
					temp->IDcode = antIDcode;
					temp->NowGameStateCode = gamecode;
					//UE_LOG(LogTemp, Warning, TEXT("Receive UserList : %s"), UTF8_TO_TCHAR(temp->Userid.c_str()));

					// �켱 ������ �ִ� �÷��̾� ������ Ž���� �� ���̵� �ִ��� ã�ƺ���.
					for (int i = 0; i < UNovaGameInstance::PlayerList.Num(); i++) {
						if (UNovaGameInstance::PlayerList[i]->IDcode == antIDcode) {
							// ���� ���̵� ������ �ش� ���̵��� �����ڵ带 �������ְ�
							UNovaGameInstance::PlayerList[i]->NowGameStateCode = gamecode;
							break;
						}
					}

					// �κ񸮽�Ʈ ������Ʈ
					UNovaGameInstance::LobbyUserListUpdate = true;
				}

			}
			// �α׾ƿ��� �� ���
			else if (state->Code() == 3) // �α׾ƿ� 3
			{
				// �Ѱܹ��� ������ �ӽ� ����
				std::string anotherUserid = state->UserID()->c_str();
				int32 gamecode = state->Code();
				int32 antIDcode = state->UserIDCode();


				GLog->Log(FString::Printf(TEXT("DataReceive = LOGOUT state / %s"), UTF8_TO_TCHAR(anotherUserid.c_str())));


				if (UNovaGameInstance::NowPlayStateCode == 0) // �κ� 0
				{
					// ������ �ִ� ������ Ž���ؼ� �ش� ���̵��� ������ �����ش�.
					for (int i = 0; i < UNovaGameInstance::PlayerList.Num(); i++) {
						if (UNovaGameInstance::PlayerList[i]->IDcode == antIDcode) {
							UNovaGameInstance::PlayerList.RemoveAt(i);
							break;
						}
					}
					UNovaGameInstance::LobbyUserListUpdate = true;
				}


				if (UNovaGameInstance::NowPlayStateCode >= 100)
				{
					// anotherUserid�� �α׾ƿ��ߴٴ� ������ �޾Ҵµ�, ���� Ŭ���̾�Ʈ�� ����Ǿ��ִ� �÷������� ���� ����Ʈ��
					// �ش� ���̵� �ִٸ� �� ���̵�� ��������Ѵ�.
					if (UNovaGameInstance::PlayingGameUserList.Contains(antIDcode))
					{
						UNovaGameInstance::PlayingGameUserList[antIDcode]->Destroy();
						UNovaGameInstance::PlayingGameUserList.Remove(antIDcode);

					}

				}


			}
	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================

	case object::Type_AllUser://===============================================================================================
	{
		auto alu = static_cast<const object::AllUser*>(objpacket->packet_type());
		// �켱 ���� ���� ������ �ִ� ��������Ʈ�� ���� ������ ������Ʈ �ϱ� ���� ����ش�.

			UNovaGameInstance::PlayerList.Empty();


			UE_LOG(LogTemp, Warning, TEXT("Receve UserList Size : %d"), alu->AllList()->size());

			for (auto list = alu->AllList()->begin(); list != alu->AllList()->end(); ++list)
			{

				// �����κ��� ���� �����͸� �ӽú����� ����
				std::string anotherUserid = list->ID()->c_str();
				int32 gamecode = list->code();
				int32 MP = list->MaxPoint();
				int32 IDCode = list->IDCode();
				UE_LOG(LogTemp, Warning, TEXT("Receve UserList  / id : %s / MP : %d"), UTF8_TO_TCHAR(anotherUserid.c_str()), MP);

				// �迭�� ������ Ŭ���� ����
				PlayerInfo* temp = new PlayerInfo();
				temp->Userid = anotherUserid;
				temp->NowGameStateCode = gamecode;
				temp->MaxPoint = MP;
				temp->IDcode = IDCode;
				// �����迭�� ����
				UNovaGameInstance::PlayerList.Add(temp);
				//UE_LOG(LogTemp, Warning, TEXT("Receive UserList3 : %s"), UTF8_TO_TCHAR(UNovaGameInstance::PlayerList[i]->Userid.c_str()));

				if (UNovaGameInstance::IDcode == list->IDCode())
				{
					UNovaGameInstance::MaxPoint = list->MaxPoint();

				}

			}
			// �κ� ������Ʈ �޼����� ������.
			UNovaGameInstance::LobbyUserListUpdate = true;
	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================

	case object::Type_RoomInfo://===============================================================================================
	{
		auto ri = static_cast<const object::RoomInfo*>(objpacket->packet_type());


			if (UNovaGameInstance::NowPlayStateCode == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Maked Room Numbe1r : %d"), UNovaGameInstance::MyRoomNumber);
				// ���� �κ� ������ ���� ���� ������ ���� ������ �濡���� �������, �� ���� ���ȣ�� �������� ����� ���� �����ۿ� ����.
				int32 roomowner = ri->RoomOwner();

				if (roomowner == UNovaGameInstance::IDcode)
				{
					UNovaGameInstance::MyRoomNumber = ri->RoomNumber();
					UNovaGameInstance::MyRoomInfo->RoomName = ri->RoomName()->c_str();
					UNovaGameInstance::MyRoomInfo->RoomNumber = ri->RoomNumber();
					UNovaGameInstance::MyRoomInfo->MaxUserNumber = ri->MaxUserNumber();
					UNovaGameInstance::MyRoomInfo->CurrentUserNumber = ri->CurrentUserNumber();
					UNovaGameInstance::MyRoomInfo->Password = ri->Password()->c_str();
					UNovaGameInstance::MyRoomInfo->RoomOwner = ri->RoomOwner();
					UNovaGameInstance::MyRoomInfo->IsThisRoomLock = ri->IsThisRoomLock();
					UNovaGameInstance::MyRoomInfo->IsThisRoomPlayingGame = ri->IsThisRoomPlayingGame();
					UNovaGameInstance::MyRoomInfo->FountainHealth = ri->FountainHealth();
					UNovaGameInstance::MyRoomInfo->user_list.Empty();

					for (auto list = ri->user_list()->begin(); list != ri->user_list()->end(); ++list)
					{
						RoomUserInfoCls* temp = new RoomUserInfoCls();
						temp->userid = list->userid()->c_str();
						temp->useridCode = list->useridCode();
						temp->MaxPoint = list->MaxPoint();
						temp->isready = list->isready();
						UNovaGameInstance::MyRoomInfo->user_list.Add(temp);
					}

					UE_LOG(LogTemp, Warning, TEXT("Maked Room Number : %d"), UNovaGameInstance::MyRoomNumber);
				}
				else // ���� ���� ������ ����, ���� ���� ���� �ƴ� ����. �� ���� �濡 �����ϴ� ���
				{
					UNovaGameInstance::MyRoomNumber = ri->RoomNumber();
					UNovaGameInstance::MyRoomInfo->RoomName = ri->RoomName()->c_str();
					UNovaGameInstance::MyRoomInfo->RoomNumber = ri->RoomNumber();
					UNovaGameInstance::MyRoomInfo->MaxUserNumber = ri->MaxUserNumber();
					UNovaGameInstance::MyRoomInfo->CurrentUserNumber = ri->CurrentUserNumber();
					UNovaGameInstance::MyRoomInfo->Password = ri->Password()->c_str();
					UNovaGameInstance::MyRoomInfo->RoomOwner = ri->RoomOwner();
					UNovaGameInstance::MyRoomInfo->IsThisRoomLock = ri->IsThisRoomLock();
					UNovaGameInstance::MyRoomInfo->IsThisRoomPlayingGame = ri->IsThisRoomPlayingGame();
					UNovaGameInstance::MyRoomInfo->FountainHealth = ri->FountainHealth();
					UNovaGameInstance::MyRoomInfo->user_list.Empty();

					for (auto list = ri->user_list()->begin(); list != ri->user_list()->end(); ++list)
					{
						RoomUserInfoCls* temp = new RoomUserInfoCls();
						temp->userid = list->userid()->c_str();
						temp->useridCode = list->useridCode();
						temp->MaxPoint = list->MaxPoint();
						temp->isready = list->isready();
						UNovaGameInstance::MyRoomInfo->user_list.Add(temp);
					}
					UE_LOG(LogTemp, Warning, TEXT("Enter Room Number : %d"), UNovaGameInstance::MyRoomNumber);

				}
			}
			else {
				// ���� ������ �濡���� ������ ������
				if (ri->RoomNumber() == UNovaGameInstance::NowPlayStateCode && !UNovaGameInstance::IsNowGamePlaying)
				{
					// ������Ʈ���ش�.
					UNovaGameInstance::MyRoomInfo->RoomName = ri->RoomName()->c_str();
					UNovaGameInstance::MyRoomInfo->RoomNumber = ri->RoomNumber();
					UNovaGameInstance::MyRoomInfo->MaxUserNumber = ri->MaxUserNumber();
					UNovaGameInstance::MyRoomInfo->CurrentUserNumber = ri->CurrentUserNumber();
					UNovaGameInstance::MyRoomInfo->Password = ri->Password()->c_str();
					UNovaGameInstance::MyRoomInfo->RoomOwner = ri->RoomOwner();
					UNovaGameInstance::MyRoomInfo->IsThisRoomLock = ri->IsThisRoomLock();
					UNovaGameInstance::MyRoomInfo->IsThisRoomPlayingGame = ri->IsThisRoomPlayingGame();
					UNovaGameInstance::MyRoomInfo->FountainHealth = ri->FountainHealth();
					UNovaGameInstance::MyRoomInfo->user_list.Empty();
					for (auto list = ri->user_list()->begin(); list != ri->user_list()->end(); ++list)
					{
						RoomUserInfoCls* temp = new RoomUserInfoCls();
						temp->userid = list->userid()->c_str();
						temp->useridCode = list->useridCode();
						temp->MaxPoint = list->MaxPoint();
						temp->isready = list->isready();
						UNovaGameInstance::MyRoomInfo->user_list.Add(temp);
					}
					UE_LOG(LogTemp, Warning, TEXT("Maked Room Number333 : %d"), UNovaGameInstance::MyRoomNumber);
					UNovaGameInstance::RoomUserListUpdate = true;
				}

				if (UNovaGameInstance::IsNowGamePlaying) {

					UNovaGameInstance::TowerHP = ri->FountainHealth();
					UNovaGameInstance::UpdateTowerHP = true;
				}
			}



	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================
	case object::Type_RoomList:
	{

		auto rl = static_cast<const object::RoomList*>(objpacket->packet_type());


			UNovaGameInstance::TempRoomInfo.Empty();

			UE_LOG(LogTemp, Warning, TEXT("RoomListUpdateGet!!"));
			for (auto list = rl->room_list()->begin(); list != rl->room_list()->end(); ++list)
			{
				RoomInfoCls* temp = new RoomInfoCls();
				temp->RoomName = list->RoomName()->c_str();
				temp->RoomNumber = list->RoomNumber();
				temp->CurrentUserNumber = list->CurrentUserNumber();
				temp->MaxUserNumber = list->MaxUserNumber();
				temp->IsThisRoomPlayingGame = list->IsThisRoomPlayingGame();
				temp->IsThisRoomLock = list->IsThisRoomLock();
				UNovaGameInstance::TempRoomInfo.Add(temp);
			}

			UNovaGameInstance::LobbyRoomListUpdate = true;
	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================

	case object::Type_GameStartInfo:
	{
		auto str = static_cast<const object::GameStartInfo*>(objpacket->packet_type());


			// ���ӹ� Ȥ�� �����÷��̸� �����ϱ� ��
			if (str->iscounting() == true && str->count() >= 0)
			{
				UNovaGameInstance::GameCounting = true;
				UNovaGameInstance::CountingNumber = str->count();
			}
	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================

	case object::Type_CheckPassword:
	{
		auto ckpw = static_cast<const object::CheckPassword*>(objpacket->packet_type());

			if (ckpw->Iscorrect() == true) {
				UNovaGameInstance::IsPasswordCorrect = true;
			}
			else
			{
				UNovaGameInstance::IsPasswordCorrect = false;
			}
			UNovaGameInstance::ReceivedPasswordCheckData = true;
	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================

	case object::Type_ChatMessage:
	{
		auto chat = static_cast<const object::ChatMessage*>(objpacket->packet_type());


			std::string msg = chat->ChatMsg()->c_str();
			std::string id = chat->SenderID()->c_str();
			//UE_LOG(LogTemp, Warning, TEXT("Write Meg : %s : %s"), UTF8_TO_TCHAR(id.c_str()), UTF8_TO_TCHAR(msg.c_str()));

			UNovaGameInstance::ReceiveChatMessage = msg;
			UNovaGameInstance::ReceiveMsgUserID = id;
			// �κ� �÷��̾� ��Ʈ�ѷ��� ƽ���ۺκп��� �޼ҵ带 �����ų �� �ֵ��� true������ �ٲ��ش�.
			UNovaGameInstance::IsNeedUpdateMessage = true;

			//lobbywidget->ChatNewMessage(UTF8_TO_TCHAR(msg.c_str()));

	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================
	case object::Type_MyData:
	{
		auto mydata = static_cast<const object::MyData*> (objpacket->packet_type());

		UNovaGameInstance::MyCharacterData->CharacterCurrentHP = static_cast<float>(mydata->HP());
		UNovaGameInstance::CurrentPoint = mydata->Point();
		UNovaGameInstance::CurrentPotion100 = mydata->Potion100();
		UNovaGameInstance::CurrentPotion50 = mydata->Potion50();

		if (UNovaGameInstance::MyCharacterData->IsPlayerAlive == false && UNovaGameInstance::MyCharacterData->CharacterCurrentHP > 0) {
			// ĳ���� ���´� �����ε�, ü���� 0�̻��� ��� -> ��Ȱ�Ѱ��
			UNovaGameInstance::MyCharacterData->IsPlayerAlive = true;
		}
	}
	break;

	case object::Type_PlayerData:
	{
		auto player = static_cast<const object::PlayerData*> (objpacket->packet_type());
		//auto player = Cast<const object::PlayerData>(objpacket->packet_type());


		//UE_LOG(LogTemp, Warning, TEXT("*----------- | HasPlayerData"));
		//UE_LOG(LogTemp, Warning, TEXT("Maybe Here!! %s  / %d"), UTF8_TO_TCHAR(objpacket->playerdata().player_name().c_str()), objpacket->playerdata().health());

		std::string CharacterName = player->CharacterName()->c_str();
		int32 characterid = player->player_name();
		int32 characterHP = player->health();
		float locx = player->playerPosition()->loc()->X();
		float locy = player->playerPosition()->loc()->Y();
		float locz = player->playerPosition()->loc()->Z();

		float rotYaw = player->playerPosition()->rot()->Yaw();
		float rotPitch = player->playerPosition()->rot()->Pitch();
		float rotRoll = player->playerPosition()->rot()->Roll();

		float vocX = player->playerPosition()->voc()->VX();
		float vocY = player->playerPosition()->voc()->VY();
		float vocZ = player->playerPosition()->voc()->VZ();

		bool isAlive = player->IsAlive();
		bool isattack = player->IsAttacking();
		int32 ACNum = player->AttackCountNumber();
		bool isskill = player->IsSkillUsing();
		int32 SKNum = player->SkillNumber();


		if (UNovaGameInstance::PlayingGameUserList.Contains(characterid)) // �̹� �÷��� ����Ʈ�� ����ִ� ��� -> ���¸� ������Ʈ ���ش�.
		{
			// ��Ȱ�� ���
			if (UNovaGameInstance::PlayingGameUserList[characterid]->IsPlayerAlive == false
				&& characterHP > 0) {
				UNovaGameInstance::PlayingGameUserList[characterid]->IsPlayerAlive = true;
				UNovaGameInstance::PlayingGameUserList[characterid]->CharacterRevival();
			}


			if (UNovaGameInstance::PlayingGameUserList[characterid]->IsPlayerAlive == true) {
				FVector CharacterLocation;
				CharacterLocation.X = locx;
				CharacterLocation.Y = locy;
				CharacterLocation.Z = locz;

				FRotator CharacterRotation;
				CharacterRotation.Yaw = rotYaw;
				CharacterRotation.Pitch = rotPitch;
				CharacterRotation.Roll = rotRoll;

				FVector CharacterVelocity;
				CharacterVelocity.X = vocX;
				CharacterVelocity.Y = vocY;
				CharacterVelocity.Z = vocZ;

				UNovaGameInstance::PlayingGameUserList[characterid]->AddMovementInput(CharacterVelocity);
				UNovaGameInstance::PlayingGameUserList[characterid]->SetActorRotation(CharacterRotation);
				UNovaGameInstance::PlayingGameUserList[characterid]->SetActorLocation(CharacterLocation);

				UNovaGameInstance::PlayingGameUserList[characterid]->IsAttacking = isattack; // �������ΰ�?
				UNovaGameInstance::PlayingGameUserList[characterid]->AttackingCountNumber = ACNum; // ī��Ʈ��ȣ �����ΰ�?
				UNovaGameInstance::PlayingGameUserList[characterid]->CharacterCurrentHP = static_cast<float>(characterHP); // ü���� ���ΰ�?
				UNovaGameInstance::PlayingGameUserList[characterid]->IsPlayerAlive = isAlive; // ����ִ°�?
				UNovaGameInstance::PlayingGameUserList[characterid]->IsSkillUsing = isskill;
				UNovaGameInstance::PlayingGameUserList[characterid]->SkillUsingNumber = SKNum;
				UNovaGameInstance::PlayingGameUserList[characterid]->AnotherPlayerAttack();//�ٸ��÷��̾ �����ϸ� �ߵ�
				UNovaGameInstance::PlayingGameUserList[characterid]->AnotherPlayer_SkillUsing();//�ٸ��÷��̾ ��ų���� �ߵ�


				// ���� ���·� ��ȭ�����ش�.
				if (UNovaGameInstance::PlayingGameUserList[characterid]->CharacterCurrentHP < 0.0f
					&& UNovaGameInstance::PlayingGameUserList[characterid]->IsPlayerAlive == true) {
					UNovaGameInstance::PlayingGameUserList[characterid]->IsPlayerAlive = false;
					UNovaGameInstance::PlayingGameUserList[characterid]->CharacterDead();
				}

			}

		}
		else // �÷��� ����Ʈ�� ���°�� -> ���������ش�.
		{


			UNovaGameInstance::TempPlayerData.CharacterName = CharacterName;
			UNovaGameInstance::TempPlayerData.player_name = characterid;
			UNovaGameInstance::TempPlayerData.health = characterHP;
			UNovaGameInstance::TempPlayerData.IsAlive = isAlive;
			UNovaGameInstance::TempPlayerData.IsAttacking = isattack;
			UNovaGameInstance::TempPlayerData.AttackCountNumber = ACNum;
			UNovaGameInstance::TempPlayerData.IsSkillUsing = isskill;
			UNovaGameInstance::TempPlayerData.SkillNumber = SKNum;


			//��ġ, ȸ��, �ӵ�
			UNovaGameInstance::TempPlayerData.Loc_X = locx;
			UNovaGameInstance::TempPlayerData.Loc_Y = locy;
			UNovaGameInstance::TempPlayerData.Loc_Z = locz;
			UNovaGameInstance::TempPlayerData.Rot_Pitch = rotPitch;
			UNovaGameInstance::TempPlayerData.Rot_Roll = rotRoll;
			UNovaGameInstance::TempPlayerData.Rot_Yaw = rotYaw;
			UNovaGameInstance::TempPlayerData.Voc_VX = vocX;
			UNovaGameInstance::TempPlayerData.Voc_VY = vocY;
			UNovaGameInstance::TempPlayerData.Voc_VZ = vocZ;

			UNovaGameInstance::GamePlayerUpdate = true;
		}
			




	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================

	case object::Type_GameResult:
	{
		auto result = static_cast<const object::GameResult*>(objpacket->packet_type());


			UNovaGameInstance::Rank1_ID = UTF8_TO_TCHAR(result->Rank1_ID()->c_str());
			UNovaGameInstance::Rank2_ID = UTF8_TO_TCHAR(result->Rank2_ID()->c_str());
			UNovaGameInstance::Rank3_ID = UTF8_TO_TCHAR(result->Rank3_ID()->c_str());
			UNovaGameInstance::Rank4_ID = UTF8_TO_TCHAR(result->Rank4_ID()->c_str());

			UNovaGameInstance::Rank1_Point = result->Rank1_Point();
			UNovaGameInstance::Rank2_Point = result->Rank2_Point();
			UNovaGameInstance::Rank3_Point = result->Rank3_Point();
			UNovaGameInstance::Rank4_Point = result->Rank4_Point();

			UNovaGameInstance::GameEndLeftTime = result->DuringDefenseTime();
			UNovaGameInstance::GameEndLeftTowerHP = result->LeftTowerHP();

			UNovaGameInstance::KilledMonsterValue = result->KilledMonster();
			UNovaGameInstance::KilledMonsterValue_Rare = result->KilledMonster_Rare();
			UNovaGameInstance::KilledMonsterValue_Boss = result->KilledMonster_Boss();
			UNovaGameInstance::DiedValue = result->TotalDied();
			UNovaGameInstance::TotalGaindDamage = result->TotalDamageToMonster();
			UNovaGameInstance::TotalReceivedDamage = result->TotalReceivedDamage();

			UE_LOG(LogTemp, Warning, TEXT("GetGameResult"));
	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================

	case object::Type_TopRanking:
	{
		UE_LOG(LogTemp, Warning, TEXT("Receve Top"));
		auto top = static_cast<const object::TopRanking*>(objpacket->packet_type());


			UNovaGameInstance::TopRanker.Empty();

			for (auto list = top->ranklist()->begin(); list != top->ranklist()->end(); ++list)
			{
				std::string rankid = list->userid()->c_str();
				int32 MP = list->point();

				PlayerInfo* temp = new PlayerInfo();
				temp->Userid = rankid;
				temp->MaxPoint = MP;
				// �����迭�� ����
				UNovaGameInstance::TopRanker.Add(temp);
			}
			UNovaGameInstance::UpdateTopRanking = true;
	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================
	case object::Type_SpawnMonsterData:
	{
		// ���� ���� �����͸� �޾����� ���͸� ���������ִ� �κ�.
		auto spawndata = static_cast<const object::SpawnMonsterData*> (objpacket->packet_type());

		


		int32 RecvMonsterID = spawndata->player_name();

		if (UNovaGameInstance::MonsterList.Contains(RecvMonsterID))
		{
			// �̹� Ŭ���̾�Ʈ���� ������ �Ǿ� �ְ�, ���� ����Ʈ�� ��ϵǾ� �ִµ�
			// �������� �� �����͸� ���� ����, Ŭ���̾�Ʈ�ʿ��� ������ �Ϸ�Ǿ��ٶ�� �޼����� ������ ���� ���
			// ������ �����Ϸ� �޼����� �����ش�.
			UNovaGameInstance::NovaSocketcli->SendSpawnOK(); // �����Ǿ��ٰ� ������ �����ش�.

		}
		else {
			// �� ���� ���� ó�� ���� ������ �Ǵ� ����̹Ƿ�, ������Ʈ �����ش�.
			UE_LOG(LogTemp, Warning, TEXT("SpawnMonster!!!!!!"));
			UNovaGameInstance::TempMonsterData.player_name = spawndata->player_name();
			UNovaGameInstance::TempMonsterData.health = spawndata->health();
			UNovaGameInstance::TempMonsterData.Loc_X = spawndata->monsterLocation()->X(); // ���⿡�� ���� Access Violation reading location ������ ����.
			UNovaGameInstance::TempMonsterData.Loc_Y = spawndata->monsterLocation()->Y();
			UNovaGameInstance::TempMonsterData.Loc_Z = spawndata->monsterLocation()->Z();
			UNovaGameInstance::TempMonsterData.MonsterType = spawndata->MonsterType();

			UNovaGameInstance::GameMonsterUpdate = true;
		}


	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================
	case object::Type_MonsterData:
	{
		// ���� �����͸� �޾� ���͸� ������Ʈ �����ִ� �κ�
		// ������Ű�� �ʴ´�.
		//UE_LOG(LogTemp, Warning, TEXT("GetMonsterData!!!!!!"));

		auto monster = static_cast<const object::MonsterData*> (objpacket->packet_type());


			//UE_LOG(LogTemp, Warning, TEXT("HasMonsterData!!"));
			int32 RecvMonsterID = monster->player_name();
			int32 monHealth = monster->health();
			int locx = monster->monsterLocation()->X(); // ���� ���� ����!!! ������ ��������!!
			int locy = monster->monsterLocation()->Y();
			int locz = monster->monsterLocation()->Z();
			bool RecvMonsterIsAlive = monster->IsAlive();
			bool isattack = monster->IsAttacking();
			int ACNumber = monster->AttackCountNumber();

			if (UNovaGameInstance::MonsterList.Contains(RecvMonsterID)) // ���� ����Ʈ�� ������ ���¸� ������Ʈ
			{
				if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
				{
					// ���� ������ ���, ���� �����͸� ������ ü�ºκи� �޴´�.
					UNovaGameInstance::MonsterList[RecvMonsterID]->MonsterCurrentHP = static_cast<float>(monHealth);
					UNovaGameInstance::MonsterList[RecvMonsterID]->IsMonsterAlive = RecvMonsterIsAlive;

				}
				else
				{
					// �̹� ������ ���� �̹Ƿ�, ���� ������ ������Ʈ ���ش�.
					FVector CharacterLocation;
					CharacterLocation.X = locx;
					CharacterLocation.Y = locy;
					CharacterLocation.Z = locz;

					UNovaGameInstance::MonsterList[RecvMonsterID]->MoveToLocation(CharacterLocation);
					UNovaGameInstance::MonsterList[RecvMonsterID]->IsAttacking = isattack;
					UNovaGameInstance::MonsterList[RecvMonsterID]->AttackingCountNumber = ACNumber;
					UNovaGameInstance::MonsterList[RecvMonsterID]->MonsterCurrentHP = static_cast<float>(monHealth);
					UNovaGameInstance::MonsterList[RecvMonsterID]->IsMonsterAlive = RecvMonsterIsAlive;
					UNovaGameInstance::MonsterList[RecvMonsterID]->AnotherPlayerAttack();
					//UE_LOG(LogTemp, Warning, TEXT("NRO : MonsterHP : %f(%d)"), UNovaGameInstance::MonsterList[i]->MonsterCurrentHP, UNovaGameInstance::MonsterList[i]->Monsterid);


				}
			}

	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================

	case object::Type_SystemMessage:
	{
		auto sysmsg = static_cast<const object::SystemMessage*>(objpacket->packet_type());
		// �ý��� �޼����� �ޱ� �����ߴµ�, ���� ȭ���� ī��Ʈ�� ������� �������ִ�.
		// �� ��� �����ֱ� ���� �κ��̴�.
		if (UNovaGameInstance::CountingNumber > 0) { // ���� ī���� ��ȣ�� ���� 0�̻��� ���
			UNovaGameInstance::CountingNumber = 0; // 0���� �ٲ��ش�.
			UNovaGameInstance::GameCounting = true; // �ٽ��ѹ� �� ������Ʈ ���ش�.
		}

			std::string msg = sysmsg->message()->str(); // TODO : str�� �ϸ� �����ٸ���?
			UNovaGameInstance::ReceiveSystemMessage = msg;
			UNovaGameInstance::SystemMessageCode = sysmsg->type();
			UNovaGameInstance::UpdateSystemMessage = true;
	}
	break;

	//===============================================================================================
	//===============================================================================================
	//===============================================================================================

	case object::Type_SystemTime:
	{
		auto systime = static_cast<const object::SystemTime*>(objpacket->packet_type());


			UNovaGameInstance::GameTime = systime->Time();
			UNovaGameInstance::UpdateGameTime = true;
			UE_LOG(LogTemp, Warning, TEXT("LeftGameTime : %d"), UNovaGameInstance::GameTime);
	}
	break;

	case object::Type_MoveMonster:
	{
		auto movemonster = static_cast<const object::MoveMonster*>(objpacket->packet_type());
			int MovingMonsterid = movemonster->WhichMonster();
			//UE_LOG(LogTemp, Warning, TEXT("Receive Move Monster : %d"), MovingMonsterid);

			UNovaGameInstance::MonsterList[MovingMonsterid]->MonsterMove = true;
			UNovaGameInstance::NovaSocketcli->SendMoveOK();
			UE_LOG(LogTemp, Warning, TEXT("Move? : %d(%d - 1: true, 0: false)"), UNovaGameInstance::MonsterList[MovingMonsterid]->Monsterid, UNovaGameInstance::MonsterList[MovingMonsterid]->MonsterMove);
	}
	break;

	case object::Type_GameEnd:
	{
		auto gameend = static_cast<const object::GameEnd*>(objpacket->packet_type());
		// ���������϶�� �޼����� �޴� ����̴�.
		// ���� �������ᰡ �ȵǾ��ٸ� ��������ش�.
		UNovaGameInstance::MyRoomInfo->IsGameWin = gameend->IsEnd();

		if (UNovaGameInstance::MyRoomInfo->IsGameEnd == false && UNovaGameInstance::MyRoomInfo->EndWidget == false) {
			UNovaGameInstance::MyRoomInfo->IsGameEnd = true;

			
		}
		else
		{
			// �̹� �����ߴµ� �� �޼����� �޴� ����, ���� �����ߴٴ� �޼����� ������ �� �������� ���� ����.
			// �ٽú����ش�.
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
			UE_LOG(LogTemp, Warning, TEXT("Send Game End 2"))

		}
	}
	break;

	case object::Type_BuyRequest:
	{
		auto buyreq = static_cast<const object::BuyRequest*>(objpacket->packet_type());
		bool result = buyreq->request();
		if (result == false) { // ���Ű����� ���
			UNovaGameInstance::ItemBuySucessful = 1; // ���Ű����ڵ� 1
		}
		else { // ���� ����
			UNovaGameInstance::ItemBuySucessful = 2; // ���ż����ڵ� 2
		}
	}
	break;

	case object::Type_RefreshItem:
	{
		auto item = static_cast<const object::RefreshItem*>(objpacket->packet_type());
		int32 P100 = item->CurrentPotion100();
		int32 P50 = item->CurrentPotion50();
		int32 Gold = item->CurrentGold();

		UNovaGameInstance::MyCurrentGold = Gold;
		UNovaGameInstance::CurrentPotion100 = P100;
		UNovaGameInstance::CurrentPotion50 = P50;
	}

	break;

	}

	
	
}




