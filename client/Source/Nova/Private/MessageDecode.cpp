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
			//		// 다른 유저가 로비에 들어온 경우
			if (state->Code() == 0 // 로비 0
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

				// 로비에 들어오는 경우는 2가지가 있음, 최초로 로그인 하는 경우와
				bool isfisrtconnect = true; // 다른 상태에 있다가 로비로 돌아오는 경우

				// 최초로 로그인하는 경우는, 접속한 유저의 정보가 나한테 없기 때문에 추가해줘야 하고,
				// 다른 상태에 있다가 로비로 돌아오는 경우는 상태정보의 변경만 해주면 된다.

				// 우선 가지고 있는 플레이어 정보를 탐색한 후 아이디가 있는지 찾아본다.
				for (int i = 0; i < UNovaGameInstance::PlayerList.Num(); i++) {
					if (UNovaGameInstance::PlayerList[i]->IDcode == antIDcode) {
						// 만약 아이디가 있으면 해당 아이디의 게임코드를 변경해주고
						//UNovaGameInstance::PlayerList[i]->NowGameStateCode = gamecode;
						UNovaGameInstance::PlayerList[i]->NowGameStateCode = gamecode;
						isfisrtconnect = false;
						break;
					}
				}
				// 최초로 로그인 하는 경우엔, 플레이어 리스트에 추가해준다.
				if (isfisrtconnect == true) {
					UE_LOG(LogTemp, Warning, TEXT("Add Lobby User List : %s"), UTF8_TO_TCHAR(temp->Userid.c_str()));
					UNovaGameInstance::PlayerList.Add(temp);
				}

				// 로비리스트 업데이트
				UNovaGameInstance::LobbyUserListUpdate = true;

				// 플레이중인 유저가 로비로 갔기 때문에, 게임에서 해당 유저의 캐릭터를 파괴시켜주는 부분
				// 그 외의 경우는 다른 상태에 있다가 로비에 들어온 경우이므로, 상태를 다시 비교한다.
				if (isfisrtconnect == false && UNovaGameInstance::IsNowGamePlaying) {
					// anotherUserid가 로비로 들어왔다는 정보를 받았는데, 나의 클라이언트에 저장되어있는 플레이중인 유저 리스트에
					// 해당 아이디가 있다면 그 아이디는 지워줘야한다. 캐릭터 제거
					if (UNovaGameInstance::PlayingGameUserList.Contains(antIDcode))
					{
						UNovaGameInstance::PlayingGameUserList[antIDcode]->Destroy();
						UNovaGameInstance::PlayingGameUserList.Remove(antIDcode);

					}

				}
			}



			//
			// 게임플레이를 하는 경우
			else if (state->Code() >= 100)
			{

				std::string anotherUserid = state->UserID()->c_str();
				int32 gamecode = state->Code();
				int32 antIDcode = state->UserIDCode();

				//GLog->Log(FString::Printf(TEXT("DataReceive = GAMEPLAY state / %s"), UTF8_TO_TCHAR(anotherUserid.c_str())));

				if (UNovaGameInstance::NowPlayStateCode == 0) // 로비 0
				{
					//AnotherPlayerInfo temp = AnotherPlayerInfo(anotherUserid, gamecode);
					PlayerInfo* temp = new PlayerInfo();
					temp->Userid = anotherUserid;
					temp->IDcode = antIDcode;
					temp->NowGameStateCode = gamecode;
					//UE_LOG(LogTemp, Warning, TEXT("Receive UserList : %s"), UTF8_TO_TCHAR(temp->Userid.c_str()));

					// 우선 가지고 있는 플레이어 정보를 탐색한 후 아이디가 있는지 찾아본다.
					for (int i = 0; i < UNovaGameInstance::PlayerList.Num(); i++) {
						if (UNovaGameInstance::PlayerList[i]->IDcode == antIDcode) {
							// 만약 아이디가 있으면 해당 아이디의 게임코드를 변경해주고
							UNovaGameInstance::PlayerList[i]->NowGameStateCode = gamecode;
							break;
						}
					}

					// 로비리스트 업데이트
					UNovaGameInstance::LobbyUserListUpdate = true;
				}

			}
			// 로그아웃을 한 경우
			else if (state->Code() == 3) // 로그아웃 3
			{
				// 넘겨받은 데이터 임시 저장
				std::string anotherUserid = state->UserID()->c_str();
				int32 gamecode = state->Code();
				int32 antIDcode = state->UserIDCode();


				GLog->Log(FString::Printf(TEXT("DataReceive = LOGOUT state / %s"), UTF8_TO_TCHAR(anotherUserid.c_str())));


				if (UNovaGameInstance::NowPlayStateCode == 0) // 로비 0
				{
					// 가지고 있는 정보를 탐색해서 해당 아이디의 정보를 지워준다.
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
					// anotherUserid가 로그아웃했다는 정보를 받았는데, 나의 클라이언트에 저장되어있는 플레이중인 유저 리스트에
					// 해당 아이디가 있다면 그 아이디는 지워줘야한다.
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
		// 우선 현재 내가 가지고 있는 유저리스트에 대한 정보를 업데이트 하기 위해 비워준다.

			UNovaGameInstance::PlayerList.Empty();


			UE_LOG(LogTemp, Warning, TEXT("Receve UserList Size : %d"), alu->AllList()->size());

			for (auto list = alu->AllList()->begin(); list != alu->AllList()->end(); ++list)
			{

				// 서버로부터 받은 데이터를 임시변수에 저장
				std::string anotherUserid = list->ID()->c_str();
				int32 gamecode = list->code();
				int32 MP = list->MaxPoint();
				int32 IDCode = list->IDCode();
				UE_LOG(LogTemp, Warning, TEXT("Receve UserList  / id : %s / MP : %d"), UTF8_TO_TCHAR(anotherUserid.c_str()), MP);

				// 배열에 저장할 클래스 생성
				PlayerInfo* temp = new PlayerInfo();
				temp->Userid = anotherUserid;
				temp->NowGameStateCode = gamecode;
				temp->MaxPoint = MP;
				temp->IDcode = IDCode;
				// 전역배열에 저장
				UNovaGameInstance::PlayerList.Add(temp);
				//UE_LOG(LogTemp, Warning, TEXT("Receive UserList3 : %s"), UTF8_TO_TCHAR(UNovaGameInstance::PlayerList[i]->Userid.c_str()));

				if (UNovaGameInstance::IDcode == list->IDCode())
				{
					UNovaGameInstance::MaxPoint = list->MaxPoint();

				}

			}
			// 로비에 업데이트 메세지를 보낸다.
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
				// 내가 로비에 있을때 받은 방의 정보가 내가 생성한 방에관한 정보라면, 이 경우는 방번호를 서버에서 만들고 보낸 정보밖에 없다.
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
				else // 받은 방의 정보를 보니, 내가 만든 방이 아닌 경우다. 이 경우는 방에 참여하는 경우
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
				// 내가 참여한 방에대한 정보를 받으면
				if (ri->RoomNumber() == UNovaGameInstance::NowPlayStateCode && !UNovaGameInstance::IsNowGamePlaying)
				{
					// 업데이트해준다.
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


			// 게임방 혹은 게임플레이를 시작하기 전
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
			// 로비 플레이어 컨트롤러가 틱동작부분에서 메소드를 실행시킬 수 있도록 true값으로 바꿔준다.
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
			// 캐릭터 상태는 죽음인데, 체력이 0이상인 경우 -> 부활한경우
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


		if (UNovaGameInstance::PlayingGameUserList.Contains(characterid)) // 이미 플레이 리스트에 들어있는 경우 -> 상태만 업데이트 해준다.
		{
			// 부활한 경우
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

				UNovaGameInstance::PlayingGameUserList[characterid]->IsAttacking = isattack; // 공격중인가?
				UNovaGameInstance::PlayingGameUserList[characterid]->AttackingCountNumber = ACNum; // 카운트번호 무엇인가?
				UNovaGameInstance::PlayingGameUserList[characterid]->CharacterCurrentHP = static_cast<float>(characterHP); // 체력은 몇인가?
				UNovaGameInstance::PlayingGameUserList[characterid]->IsPlayerAlive = isAlive; // 살아있는가?
				UNovaGameInstance::PlayingGameUserList[characterid]->IsSkillUsing = isskill;
				UNovaGameInstance::PlayingGameUserList[characterid]->SkillUsingNumber = SKNum;
				UNovaGameInstance::PlayingGameUserList[characterid]->AnotherPlayerAttack();//다른플레이어가 공격하면 발동
				UNovaGameInstance::PlayingGameUserList[characterid]->AnotherPlayer_SkillUsing();//다른플레이어가 스킬사용시 발동


				// 죽은 상태로 변화시켜준다.
				if (UNovaGameInstance::PlayingGameUserList[characterid]->CharacterCurrentHP < 0.0f
					&& UNovaGameInstance::PlayingGameUserList[characterid]->IsPlayerAlive == true) {
					UNovaGameInstance::PlayingGameUserList[characterid]->IsPlayerAlive = false;
					UNovaGameInstance::PlayingGameUserList[characterid]->CharacterDead();
				}

			}

		}
		else // 플레이 리스트에 없는경우 -> 스폰시켜준다.
		{


			UNovaGameInstance::TempPlayerData.CharacterName = CharacterName;
			UNovaGameInstance::TempPlayerData.player_name = characterid;
			UNovaGameInstance::TempPlayerData.health = characterHP;
			UNovaGameInstance::TempPlayerData.IsAlive = isAlive;
			UNovaGameInstance::TempPlayerData.IsAttacking = isattack;
			UNovaGameInstance::TempPlayerData.AttackCountNumber = ACNum;
			UNovaGameInstance::TempPlayerData.IsSkillUsing = isskill;
			UNovaGameInstance::TempPlayerData.SkillNumber = SKNum;


			//위치, 회전, 속도
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
				// 전역배열에 저장
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
		// 몬스터 스폰 데이터를 받았을때 몬스터를 스폰시켜주는 부분.
		auto spawndata = static_cast<const object::SpawnMonsterData*> (objpacket->packet_type());

		


		int32 RecvMonsterID = spawndata->player_name();

		if (UNovaGameInstance::MonsterList.Contains(RecvMonsterID))
		{
			// 이미 클라이언트에서 스폰이 되어 있고, 몬스터 리스트에 등록되어 있는데
			// 서버에서 이 데이터를 보낸 경우는, 클라이언트쪽에서 스폰이 완료되었다라는 메세지를 보내지 않은 경우
			// 서버에 스폰완료 메세지를 보내준다.
			UNovaGameInstance::NovaSocketcli->SendSpawnOK(); // 스폰되었다고 서버에 보내준다.

		}
		else {
			// 그 외의 경우는 처음 몬스터 스폰이 되는 경우이므로, 업데이트 시켜준다.
			UE_LOG(LogTemp, Warning, TEXT("SpawnMonster!!!!!!"));
			UNovaGameInstance::TempMonsterData.player_name = spawndata->player_name();
			UNovaGameInstance::TempMonsterData.health = spawndata->health();
			UNovaGameInstance::TempMonsterData.Loc_X = spawndata->monsterLocation()->X(); // 여기에서 자주 Access Violation reading location 에러가 난다.
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
		// 몬스터 데이터를 받아 몬스터를 업데이트 시켜주는 부분
		// 스폰시키지 않는다.
		//UE_LOG(LogTemp, Warning, TEXT("GetMonsterData!!!!!!"));

		auto monster = static_cast<const object::MonsterData*> (objpacket->packet_type());


			//UE_LOG(LogTemp, Warning, TEXT("HasMonsterData!!"));
			int32 RecvMonsterID = monster->player_name();
			int32 monHealth = monster->health();
			int locx = monster->monsterLocation()->X(); // 에러 에러 에러!!! 요주의 에러라인!!
			int locy = monster->monsterLocation()->Y();
			int locz = monster->monsterLocation()->Z();
			bool RecvMonsterIsAlive = monster->IsAlive();
			bool isattack = monster->IsAttacking();
			int ACNumber = monster->AttackCountNumber();

			if (UNovaGameInstance::MonsterList.Contains(RecvMonsterID)) // 몬스터 리스트에 있으면 상태만 업데이트
			{
				if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
				{
					// 내가 방장인 경우, 몬스터 데이터를 받을때 체력부분만 받는다.
					UNovaGameInstance::MonsterList[RecvMonsterID]->MonsterCurrentHP = static_cast<float>(monHealth);
					UNovaGameInstance::MonsterList[RecvMonsterID]->IsMonsterAlive = RecvMonsterIsAlive;

				}
				else
				{
					// 이미 스폰된 몬스터 이므로, 몬스터 정보만 업데이트 해준다.
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
		// 시스템 메세지를 받기 시작했는데, 아직 화면의 카운트가 사라지지 않을수있다.
		// 이 경우 없애주기 위한 부분이다.
		if (UNovaGameInstance::CountingNumber > 0) { // 게임 카운팅 번호가 아직 0이상인 경우
			UNovaGameInstance::CountingNumber = 0; // 0으로 바꿔준다.
			UNovaGameInstance::GameCounting = true; // 다시한번 더 업데이트 해준다.
		}

			std::string msg = sysmsg->message()->str(); // TODO : str로 하면 뭐가다를까?
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
		// 게임종료하라는 메세지를 받는 경우이다.
		// 아직 게임종료가 안되었다만 종료시켜준다.
		UNovaGameInstance::MyRoomInfo->IsGameWin = gameend->IsEnd();

		if (UNovaGameInstance::MyRoomInfo->IsGameEnd == false && UNovaGameInstance::MyRoomInfo->EndWidget == false) {
			UNovaGameInstance::MyRoomInfo->IsGameEnd = true;

			
		}
		else
		{
			// 이미 종료했는데 이 메세지를 받는 경우는, 내가 종료했다는 메세지가 서버에 잘 도착하지 않은 경우다.
			// 다시보내준다.
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
			UE_LOG(LogTemp, Warning, TEXT("Send Game End 2"))

		}
	}
	break;

	case object::Type_BuyRequest:
	{
		auto buyreq = static_cast<const object::BuyRequest*>(objpacket->packet_type());
		bool result = buyreq->request();
		if (result == false) { // 구매거절된 경우
			UNovaGameInstance::ItemBuySucessful = 1; // 구매거절코드 1
		}
		else { // 구매 성공
			UNovaGameInstance::ItemBuySucessful = 2; // 구매성공코드 2
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




