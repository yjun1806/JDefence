// Fill out your copyright notice in the Description page of Project Settings.

#include "NovaSocketListener.h"
#include "NovaSocketClient.h"
#include <string>
#include "NovaGameInstance.h"
#include "LobbyWidget.h"
#include "PlayerInfo.h"
#include "Engine/World.h"
#include "MessageDecode.h"
#include "Async.h"
#include "LobbyPlayerController.h"
#include "NovaPlayerController.h"
#include "flatbuffers/flatbuffers.h"
#include "Flatbuf/FlatPacket_generated.h"

NovaSocketListener* NovaSocketListener::Runnable = nullptr;

bool NovaSocketListener::bThreadRun = false;

NovaSocketListener::NovaSocketListener()
{
	Thread = FRunnableThread::Create(this, TEXT("TCP_RECEIVER"), 0, TPri_BelowNormal);
}

NovaSocketListener::~NovaSocketListener()
{
	delete Thread;
	Thread = nullptr;
}

bool NovaSocketListener::Init()
{
	//lobbywidget = NewObject<ULobbyWidget>(ULobbyWidget::StaticClass());
	bThreadRun = true;
	return true;
}

uint32 NovaSocketListener::Run()
{
	/*TArray<uint8> ReceivedDataTmp;
	int32 FirstSize = 0;
	bool IsFirstPacket = false;*/

	GLog->Log("NovaSocketListener::Run()");
	while (bThreadRun)
	{
		//GLog->Log("NovaSocketListener::RUN()");
		// Check for a connection
		if (UNovaGameInstance::bIsConnected == false) // No connection
		{
			FPlatformProcess::Sleep(1.f); // Flow rate, slow
		}
		else
		{
			//FPlatformProcess::Sleep(0.01f);


			//Binary Array!
			TArray<uint8> ReceivedData;

			uint32 Size;

			while (UNovaGameInstance::GetNovaSocket()->HasPendingData(Size)) // While there is something to read
			{
				ReceivedData.Init(FMath::Min(Size, 65507u), Size);

				int32 Read = 0;
				//bool nRecv = UNovaGameInstance::GetNovaSocket()->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
				UNovaGameInstance::GetNovaSocket()->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
				//GLog->Log(FString::Printf(TEXT("Data Read! ReceivedData.Num %d"), ReceivedData.Num()) + " | NovaSocketListener::Run");

				if (ReceivedData.Num() > 0) {

					// ���� �����͸� ���ۿ� ��´�.
					//uint8_t* rbuffer = ReceivedData.GetData();

					TArray<uint8> CopyBuffer;
					CopyBuffer.Append(ReceivedData);
					
					//auto objpacket = object::GetPacket(ReceivedData.GetData()); // �������� ���� ������ ������ȭ
					//

					//if (objpacket->packet_type_type() > object::Type_MAX || objpacket->packet_type_type() < object::Type_MIN) {
					//	GLog->Log(FString::Printf(TEXT("OutOfBound Exeception!!")));
					//	continue;
					//}

					//GLog->Log(FString::Printf(TEXT("Data Read! Type %d"), objpacket->packet_type_type()) + " | NovaSocketListener::Run");

					
					AsyncTask(ENamedThreads::GameThread, [CopyBuffer]() {
						//GLog->Log(FString::Printf(TEXT("Data Read! Type2 %d"), objpacket->packet_type_type()) + " | NovaSocketListener::Run");

						UMessageDecode * Handler = NewObject<UMessageDecode>(UMessageDecode::StaticClass());
						Handler->ObjectPacketDecoding(CopyBuffer);
					});
					//

					


					//// �׽�Ʈ
					//UE_LOG(LogTemp, Warning, TEXT("GetDataReceive!! %d"), MsgType);
				//	
					/*
					auto MsgType = objpacket->packet_type_type();

					switch (MsgType) 
					{

						//===============================================================================================
						//===============================================================================================
						//===============================================================================================
						
						case object::Type_GameState: //===============================================================================================
						{
							auto state = static_cast<const object::GameState*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [state]() {


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
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_AllUser://===============================================================================================
						{
							auto alu = static_cast<const object::AllUser*>(objpacket->packet_type());
							// �켱 ���� ���� ������ �ִ� ��������Ʈ�� ���� ������ ������Ʈ �ϱ� ���� ����ش�.
							AsyncTask(ENamedThreads::GameThread, [alu]() {

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
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_RoomInfo://===============================================================================================
						{
							auto ri = static_cast<const object::RoomInfo*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [ri]() {

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

							});


						}
						break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================
						case object::Type_RoomList:
						{

							auto rl = static_cast<const object::RoomList*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [rl]() {

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
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_GameStartInfo:
						{
							auto str = static_cast<const object::GameStartInfo*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [str]() {

								// ���ӹ� Ȥ�� �����÷��̸� �����ϱ� ��
								if (str->iscounting() == true && str->count() >= 0)
								{
									UNovaGameInstance::GameCounting = true;
									UNovaGameInstance::CountingNumber = str->count();
								}
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_CheckPassword:
						{
							auto ckpw = static_cast<const object::CheckPassword*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [ckpw]() {
								if (ckpw->Iscorrect() == true) {
									UNovaGameInstance::IsPasswordCorrect = true;
								}
								else
								{
									UNovaGameInstance::IsPasswordCorrect = false;
								}
								UNovaGameInstance::ReceivedPasswordCheckData = true;
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_ChatMessage:
						{
							auto chat = static_cast<const object::ChatMessage*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [chat]() {

								std::string msg = chat->ChatMsg()->c_str();
								std::string id = chat->SenderID()->c_str();
								//UE_LOG(LogTemp, Warning, TEXT("Write Meg : %s : %s"), UTF8_TO_TCHAR(id.c_str()), UTF8_TO_TCHAR(msg.c_str()));

								UNovaGameInstance::ReceiveChatMessage = msg;
								UNovaGameInstance::ReceiveMsgUserID = id;
								// �κ� �÷��̾� ��Ʈ�ѷ��� ƽ���ۺκп��� �޼ҵ带 �����ų �� �ֵ��� true������ �ٲ��ش�.
								UNovaGameInstance::IsNeedUpdateMessage = true;

								//lobbywidget->ChatNewMessage(UTF8_TO_TCHAR(msg.c_str()));
							});

						}
						break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================


						case object::Type_PlayerData:
						{
							auto player = static_cast<const object::PlayerData*> (objpacket->packet_type());
							//auto player = Cast<const object::PlayerData>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [player]() {

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
								int32 totalP = player->TotalPoint();

								// �� ������ ������Ʈ �ؾ� �ϴ� �κ��� �ڽ��� ü��(�ֳĸ� �������� �������ֹǷ�, ��Ҵ��� �׾����� ��)
								if (UNovaGameInstance::IDcode == characterid)
								{
									//UE_LOG(LogTemp, Warning, TEXT("MyData %d"), UNovaGameInstance::IDcode);

									UNovaGameInstance::MyCharacterData->CharacterCurrentHP = static_cast<float>(characterHP);
									UNovaGameInstance::CurrentPoint = totalP;
									

								}
								else // ���� �����Ͱ� �� ������ �ƴ� ���, �ٸ� �������� ������ ���޹��� ���̴�.
								{
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



					
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_GameResult:
						{
							auto result = static_cast<const object::GameResult*>(objpacket->packet_type());
							AsyncTask(ENamedThreads::GameThread, [result]() {


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
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_TopRanking:
						{
							UE_LOG(LogTemp, Warning, TEXT("Receve Top"));
							auto top = static_cast<const object::TopRanking*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [top]() {

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
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================
						case object::Type_SpawnMonsterData:
						{
							// ���� ���� �����͸� �޾����� ���͸� ���������ִ� �κ�.
							auto spawndata = static_cast<const object::SpawnMonsterData*> (objpacket->packet_type());
							
								AsyncTask(ENamedThreads::GameThread, [spawndata]() {
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
								});
							

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

							AsyncTask(ENamedThreads::GameThread, [monster]() {

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
										UNovaGameInstance::MonsterList[RecvMonsterID]->AttackingCountNumber =ACNumber;
										UNovaGameInstance::MonsterList[RecvMonsterID]->MonsterCurrentHP = static_cast<float>(monHealth);
										UNovaGameInstance::MonsterList[RecvMonsterID]->IsMonsterAlive = RecvMonsterIsAlive;
										UNovaGameInstance::MonsterList[RecvMonsterID]->AnotherPlayerAttack();
										//UE_LOG(LogTemp, Warning, TEXT("NRO : MonsterHP : %f(%d)"), UNovaGameInstance::MonsterList[i]->MonsterCurrentHP, UNovaGameInstance::MonsterList[i]->Monsterid);


									}
								}
													
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_SystemMessage:
						{
							auto sysmsg = static_cast<const object::SystemMessage*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [sysmsg]() {

								std::string msg = sysmsg->message()->str(); // TODO : str�� �ϸ� �����ٸ���?
								UNovaGameInstance::ReceiveSystemMessage = msg;
								UNovaGameInstance::SystemMessageCode = sysmsg->type();
								UNovaGameInstance::UpdateSystemMessage = true;
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_SystemTime:
						{
							auto systime = static_cast<const object::SystemTime*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [systime]() {

								UNovaGameInstance::GameTime = systime->Time();
								UNovaGameInstance::UpdateGameTime = true;
								UE_LOG(LogTemp, Warning, TEXT("LeftGameTime : %d"), UNovaGameInstance::GameTime);
							});
						}
							break;

						case object::Type_MoveMonster:
						{
							auto movemonster = static_cast<const object::MoveMonster*>(objpacket->packet_type());
							AsyncTask(ENamedThreads::GameThread, [movemonster]() {
								int MovingMonsterid = movemonster->WhichMonster();
								//UE_LOG(LogTemp, Warning, TEXT("Receive Move Monster : %d"), MovingMonsterid);
											
								UNovaGameInstance::MonsterList[MovingMonsterid]->MonsterMove = true;
								UNovaGameInstance::NovaSocketcli->SendMoveOK();
								UE_LOG(LogTemp, Warning, TEXT("Move? : %d(%d - 1: true, 0: false)"), UNovaGameInstance::MonsterList[MovingMonsterid]->Monsterid, UNovaGameInstance::MonsterList[MovingMonsterid]->MonsterMove);
							});
						}
						break;

					}
					*/
					
					/*
					if (UNovaGameInstance::NowPlayStateCode == 0) { // �κ� 0

						//// ������濡 �����ϴ� ���, �Է��� �н����� ������ ���� -> (����) �������� ��й�ȣ�� ��ġ�ϴ��� üũ �� ����
						if (MsgType == object::Type_CheckPassword)
						{
							auto ckpw = static_cast<const object::CheckPassword*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [ckpw]() {
								if (ckpw->Iscorrect() == true) {
									UNovaGameInstance::IsPasswordCorrect = true;
								}
								else
								{
									UNovaGameInstance::IsPasswordCorrect = false;
								}
								UNovaGameInstance::ReceivedPasswordCheckData = true;
							});

						}


						//��ŷ���� ��ư�� ������ �������� �����͸� ���� ���
						else if (MsgType == object::Type_TopRanking)
						{
							UE_LOG(LogTemp, Warning, TEXT("Receve Top"));
							auto top = static_cast<const object::TopRanking*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [top]() {

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
							});

						}


						//��������Ʈ�� ���� ������ ����ִٸ�
						else if (MsgType == object::Type_AllUser)
						{
							auto alu = static_cast<const object::AllUser*>(objpacket->packet_type());
							// �켱 ���� ���� ������ �ִ� ��������Ʈ�� ���� ������ ������Ʈ �ϱ� ���� ����ش�.
							AsyncTask(ENamedThreads::GameThread, [alu]() {

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
							});
						}

						// ä�ø޼����� ���� ���
						else if (MsgType == object::Type_ChatMessage) {
							auto chat = static_cast<const object::ChatMessage*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [chat]() {

								std::string msg = chat->ChatMsg()->c_str();
								std::string id = chat->SenderID()->c_str();
								//UE_LOG(LogTemp, Warning, TEXT("Write Meg : %s : %s"), UTF8_TO_TCHAR(id.c_str()), UTF8_TO_TCHAR(msg.c_str()));

								UNovaGameInstance::ReceiveChatMessage = msg;
								UNovaGameInstance::ReceiveMsgUserID = id;
								// �κ� �÷��̾� ��Ʈ�ѷ��� ƽ���ۺκп��� �޼ҵ带 �����ų �� �ֵ��� true������ �ٲ��ش�.
								UNovaGameInstance::IsNeedUpdateMessage = true;

								//lobbywidget->ChatNewMessage(UTF8_TO_TCHAR(msg.c_str()));
							});

						}

						else if (MsgType == object::Type_RoomInfo)
						{
							auto ri = static_cast<const object::RoomInfo*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [ri]() {

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
							});
						}

						else if (MsgType == object::Type_RoomList)
						{
							auto rl = static_cast<const object::RoomList*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [rl]() {

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
								//UNovaGameInstance::LobbyUserListUpdate = true;
							});

						}
					}

					else if (UNovaGameInstance::NowPlayStateCode >= 100)
					{
						//		
						//		// ���� ��������� �޴� ���
						if (MsgType == object::Type_GameResult) {
							auto result = static_cast<const object::GameResult*>(objpacket->packet_type());
							AsyncTask(ENamedThreads::GameThread, [result]() {


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

								UE_LOG(LogTemp, Warning, TEXT("GetGameResult"));
							});
						
						}
						//
						//		// ���ӹ濡 �������ε�, ���ӽ�ŸƮ ������ ���� �޼����� �޴� ���
						//		// ��, �� ���� ������ ���� ���� ��ư�� ������ ������ �����Ϸ��� ����̴�.
						else if (MsgType == object::Type_GameStartInfo)
						{
							auto str = static_cast<const object::GameStartInfo*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [str]() {

								// ���ӹ� Ȥ�� �����÷��̸� �����ϱ� ��
								if (str->iscounting() == true && str->count() >= 0)
								{
									UNovaGameInstance::GameCounting = true;
									UNovaGameInstance::CountingNumber = str->count();
								}
							});
						}
						//
						else if (MsgType == object::Type_RoomInfo)
						{
							auto ri = static_cast<const object::RoomInfo*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [ri]() {

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

							});
							
						}
						//
						//		// ä�ø޼����� ���� ���
						else if (MsgType == object::Type_ChatMessage) {
							auto chat = static_cast<const object::ChatMessage*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [chat]() {

								std::string msg = chat->ChatMsg()->c_str();
								std::string id = chat->SenderID()->c_str();
								//UE_LOG(LogTemp, Warning, TEXT("Write Meg : %s : %s"), UTF8_TO_TCHAR(id.c_str()), UTF8_TO_TCHAR(msg.c_str()));

								UNovaGameInstance::ReceiveChatMessage = msg;
								UNovaGameInstance::ReceiveMsgUserID = id;
								// �κ� �÷��̾� ��Ʈ�ѷ��� ƽ���ۺκп��� �޼ҵ带 �����ų �� �ֵ��� true������ �ٲ��ش�.
								UNovaGameInstance::IsNeedUpdateMessage = true;

								//lobbywidget->ChatNewMessage(UTF8_TO_TCHAR(msg.c_str()));
							});
						
						}
						
						// �ý��� �޼����� �޴� ���
						else if (MsgType == object::Type_SystemMessage)
						{
							auto sysmsg = static_cast<const object::SystemMessage*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [sysmsg]() {

								std::string msg = sysmsg->message()->c_str();
								UNovaGameInstance::ReceiveSystemMessage = msg;
								UNovaGameInstance::SystemMessageCode = sysmsg->type();
								UNovaGameInstance::UpdateSystemMessage = true;
							});
						}
						
						// ���ӽð��޼����� �޴� ���
						else if (MsgType == object::Type_SystemTime) {
							auto systime = static_cast<const object::SystemTime*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [systime]() {

								UNovaGameInstance::GameTime = systime->Time();
								UNovaGameInstance::UpdateGameTime = true;
								UE_LOG(LogTemp, Warning, TEXT("LeftGameTime : %d"), UNovaGameInstance::GameTime);
							});
						}
						//

						// ĳ���� �����͸� �޴� ���
						else if (MsgType == object::Type_PlayerData)
						{
							auto player = static_cast<const object::PlayerData*> (objpacket->packet_type());
							//auto player = Cast<const object::PlayerData>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [player]() {

								//UE_LOG(LogTemp, Warning, TEXT("*----------- | HasPlayerData"));
								//UE_LOG(LogTemp, Warning, TEXT("Maybe Here!! %s  / %d"), UTF8_TO_TCHAR(objpacket->playerdata().player_name().c_str()), objpacket->playerdata().health());


								// �� ������ ������Ʈ �ؾ� �ϴ� �κ��� �ڽ��� ü��(�ֳĸ� �������� �������ֹǷ�, ��Ҵ��� �׾����� ��)
								if (UNovaGameInstance::IDcode == player->player_name())
								{
									UE_LOG(LogTemp, Warning, TEXT("MyData %d"), UNovaGameInstance::IDcode);

									UNovaGameInstance::MyCharacterData->CharacterCurrentHP = static_cast<float>(player->health());
									UNovaGameInstance::KilledMonsterValue = player->KilledMonster();
									UNovaGameInstance::KilledMonsterValue_Rare = player->KilledMonster_Rare();
									UNovaGameInstance::KilledMonsterValue_Boss = player->KilledMonster_Boss();
									UNovaGameInstance::DiedValue = player->TotalDied();
									UNovaGameInstance::TotalReceivedDamage = player->TotalReceivedDamage();
									UNovaGameInstance::TotalGaindDamage = player->TotalDamageToMonster();

								}


								int32 characterid = player->player_name();
								// ������ �����ʹ� �迭�� �߰���Ű��, ������ �����ʹ� ������Ʈ ��Ų��.
								bool CharacterSpawn = false;
								UE_LOG(LogTemp, Warning, TEXT("False?!!!!!! %d"), CharacterSpawn);

								for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++)
								{
									// �迭 ���ο� �����͸� ���� ĳ���� �̸��� ���� ĳ���Ͱ� �����ϴ� ���
									if (UNovaGameInstance::PlayingGameUserList[i]->CharacterID == characterid)
									{
										//UE_LOG(LogTemp, Warning, TEXT("*----------- | HasPlayerData"));
										CharacterSpawn = true;
										break;
									}
								}

								UE_LOG(LogTemp, Warning, TEXT("False?!!!!!! %d"),CharacterSpawn);


								if (CharacterSpawn == false)
								{
									UE_LOG(LogTemp, Warning, TEXT("Spawn Char!!!!!! %d / %d "), UNovaGameInstance::PlayingGameUserList.Num(), player->player_name());
									// ���� �� ȭ�鿡 �����͸� ���� ĳ���͸� ������������ ���� ����̴�.
									// ����ġ�� true�� �ٲ��ص� ĳ���͸� ���������ش�.
									//ANovaCharacter cTemp = new ANovaCharacter();

									//UNovaGameInstance::PlayingGameUserList.Add();
									//UNovaGameInstance::TempPlayerData = new PlayerDataClass();
									

									UNovaGameInstance::TempPlayerData->CharacterName = player->CharacterName()->c_str();
									UNovaGameInstance::TempPlayerData->player_name = player->player_name();
									UNovaGameInstance::TempPlayerData->health = player->health();
									UNovaGameInstance::TempPlayerData->IsAlive = player->IsAlive();
									UNovaGameInstance::TempPlayerData->IsAttacking = player->IsAttacking();
									UNovaGameInstance::TempPlayerData->AttackCountNumber = player->AttackCountNumber();
									UNovaGameInstance::TempPlayerData->IsSkillUsing = player->IsSkillUsing();
									UNovaGameInstance::TempPlayerData->SkillNumber = player->SkillNumber();
									UNovaGameInstance::TempPlayerData->TotalReceivedDamage = player->TotalReceivedDamage();
									UNovaGameInstance::TempPlayerData->TotalDied = player->TotalDied();
									UNovaGameInstance::TempPlayerData->TotalDamageToMonster = player->TotalDamageToMonster();
									UNovaGameInstance::TempPlayerData->KilledMonster = player->KilledMonster();
									UNovaGameInstance::TempPlayerData->KilledMonster_Rare = player->KilledMonster_Rare();
									UNovaGameInstance::TempPlayerData->KilledMonster_Boss = player->KilledMonster_Boss();
									UNovaGameInstance::TempPlayerData->Loc_X = player->playerPosition()->loc()->X();
									UNovaGameInstance::TempPlayerData->Loc_Y = player->playerPosition()->loc()->Y();
									UNovaGameInstance::TempPlayerData->Loc_Z = player->playerPosition()->loc()->Z();
									UNovaGameInstance::TempPlayerData->Rot_Pitch = player->playerPosition()->rot()->Pitch();
									UNovaGameInstance::TempPlayerData->Rot_Roll = player->playerPosition()->rot()->Roll();
									UNovaGameInstance::TempPlayerData->Rot_Yaw = player->playerPosition()->rot()->Yaw();
									UNovaGameInstance::TempPlayerData->Voc_VX = player->playerPosition()->voc()->VX();
									UNovaGameInstance::TempPlayerData->Voc_VY = player->playerPosition()->voc()->VY();
									UNovaGameInstance::TempPlayerData->Voc_VZ = player->playerPosition()->voc()->VZ();

									UNovaGameInstance::GamePlayerUpdate = true;
								}
								else
								{



									// �̹� ������ ĳ���� �̹Ƿ�, ĳ������ ������ ������Ʈ ���ش�.
									//UNovaGameInstance::TempPlayerData = objpacket->playerdata();
									// �� �κ��� �ڽ��� ������ ������Ʈ �� �ʿ䰡 ����. 
									for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++) {
										// ��Ȱ�� ���
										if (UNovaGameInstance::PlayingGameUserList[i]->CharacterID == player->player_name()
											&& UNovaGameInstance::PlayingGameUserList[i]->IsPlayerAlive == false
											&& player->health() > 0) {
											UNovaGameInstance::PlayingGameUserList[i]->IsPlayerAlive = true;
											UNovaGameInstance::PlayingGameUserList[i]->CharacterRevival();
										}


										if (UNovaGameInstance::PlayingGameUserList[i]->CharacterID == player->player_name()
											&& UNovaGameInstance::PlayingGameUserList[i]->IsPlayerAlive == true) {
											FVector CharacterLocation;
											CharacterLocation.X = player->playerPosition()->loc()->X();
											CharacterLocation.Y = player->playerPosition()->loc()->Y();
											CharacterLocation.Z = player->playerPosition()->loc()->Z();

											FRotator CharacterRotation;
											CharacterRotation.Yaw = player->playerPosition()->rot()->Yaw();
											CharacterRotation.Pitch = player->playerPosition()->rot()->Pitch();
											CharacterRotation.Roll = player->playerPosition()->rot()->Roll();

											FVector CharacterVelocity;
											CharacterVelocity.X = player->playerPosition()->voc()->VX();
											CharacterVelocity.Y = player->playerPosition()->voc()->VY();
											CharacterVelocity.Z = player->playerPosition()->voc()->VZ();

											UNovaGameInstance::PlayingGameUserList[i]->AddMovementInput(CharacterVelocity);
											UNovaGameInstance::PlayingGameUserList[i]->SetActorRotation(CharacterRotation);
											UNovaGameInstance::PlayingGameUserList[i]->SetActorLocation(CharacterLocation);

											UNovaGameInstance::PlayingGameUserList[i]->IsAttacking = player->IsAttacking(); // �������ΰ�?
											UNovaGameInstance::PlayingGameUserList[i]->AttackingCountNumber = player->AttackCountNumber(); // ī��Ʈ��ȣ �����ΰ�?
											UNovaGameInstance::PlayingGameUserList[i]->CharacterCurrentHP = static_cast<float>(player->health()); // ü���� ���ΰ�?
											UNovaGameInstance::PlayingGameUserList[i]->IsPlayerAlive = player->IsAlive(); // ����ִ°�?
											UNovaGameInstance::PlayingGameUserList[i]->IsSkillUsing = player->IsSkillUsing();
											UNovaGameInstance::PlayingGameUserList[i]->SkillUsingNumber = player->SkillNumber();
											UNovaGameInstance::PlayingGameUserList[i]->AnotherPlayerAttack();//�ٸ��÷��̾ �����ϸ� �ߵ�
											UNovaGameInstance::PlayingGameUserList[i]->AnotherPlayer_SkillUsing();//�ٸ��÷��̾ ��ų���� �ߵ�

											//UNovaGameInstance::PlayingGameUserList[i]->UpdateCharacterHPbar();//�ٸ��÷��̾� ü�¾�����Ʈ

											if (UNovaGameInstance::PlayingGameUserList[i]->CharacterCurrentHP < 0.0f
												&& UNovaGameInstance::PlayingGameUserList[i]->IsPlayerAlive == true) {
												UNovaGameInstance::PlayingGameUserList[i]->IsPlayerAlive = false;
												UNovaGameInstance::PlayingGameUserList[i]->CharacterDead();
											}




										}
									}
								}
							});
						}
						//
						else if (MsgType == object::Type_MonsterData && UNovaGameInstance::IsNowGamePlaying)
						{
							auto monster = static_cast<const object::MonsterData*> (objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [monster]() {

								//UE_LOG(LogTemp, Warning, TEXT("HasMonsterData!!"));
								int32 RecvMonsterID = monster->player_name();
								bool RecvMonsterIsAlive = monster->IsAlive();


								// ������ �����ʹ� �迭�� �߰���Ű��, ������ �����ʹ� ������Ʈ ��Ų��.
								bool isFirstUpdate = false;
								for (int i = 0; i < UNovaGameInstance::MonsterList.Num(); i++)
								{
									// �迭 ���ο� �����͸� ���� �̸��� ���� ���Ͱ� �����ϴ� ���
									if (UNovaGameInstance::MonsterList[i]->Monsterid == RecvMonsterID)
									{
										isFirstUpdate = true;
										break;
									}
								}


								if (isFirstUpdate == false)
								{
									// ���� �� ȭ�鿡 �����͸� ���� ���͸� ������������ ���� ����̴�.
									// ����ġ�� true�� �ٲ��ص� ���͸� ���������ش�.
									//ANovaCharacter cTemp = new ANovaCharacter();

									//UNovaGameInstance::PlayingGameUserList.Add();
									UE_LOG(LogTemp, Warning, TEXT("SpawnMonster!!!!!!"));

									UNovaGameInstance::TempMonsterData = new MonsterDataClass();
									UNovaGameInstance::TempMonsterData->player_name = monster->player_name();
									UNovaGameInstance::TempMonsterData->health = monster->health();
									UNovaGameInstance::TempMonsterData->IsAlive = monster->IsAlive();
									UNovaGameInstance::TempMonsterData->IsAttacking = monster->IsAttacking();
									UNovaGameInstance::TempMonsterData->AttackCountNumber = monster->AttackCountNumber();
									UNovaGameInstance::TempMonsterData->MonsterType = monster->MonsterType();
									UNovaGameInstance::TempMonsterData->Loc_X = monster->monsterposition()->loc()->X();
									UNovaGameInstance::TempMonsterData->Loc_Y = monster->monsterposition()->loc()->Y();
									UNovaGameInstance::TempMonsterData->Loc_Z = monster->monsterposition()->loc()->Z();
									UNovaGameInstance::GameMonsterUpdate = true;
								}
								else
								{
									if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
									{
										// ���� ������ ���, ���� �����͸� ������ ü�ºκи� �޴´�.
										for (int i = 0; i < UNovaGameInstance::MonsterList.Num(); i++) {

											if (UNovaGameInstance::MonsterList[i]->Monsterid == monster->player_name()) {
												UNovaGameInstance::MonsterList[i]->MonsterCurrentHP = static_cast<float>(monster->health());
												UNovaGameInstance::MonsterList[i]->IsMonsterAlive = RecvMonsterIsAlive;
											//UE_LOG(LogTemp, Warning, TEXT("RO : MonsterHP : %d(%d)"), monster->IsAlive(), UNovaGameInstance::MonsterList[i]->Monsterid);
												// ���� ���� ü���� 0�ε�, ����ִ� ���¶�� �׾��ٴ� ���·� �ٲ��ְ�, ��ü������� ��ȭ�����ش�.
												//if (UNovaGameInstance::MonsterList[i]->MonsterCurrentHP <= 0.0f && UNovaGameInstance::MonsterList[i]->IsMonsterAlive == true) {
														
												//UNovaGameInstance::MonsterList[i]->IsMonsterAlive = false; // ���Ͱ� �׾��ٴ� ���·� �ٲ��ش�.
												//UNovaGameInstance::MonsterList[i]->MonsterDead();
												//UNovaGameInstance::MonsterList[i]->DiedAnim = true;


													//UNovaGameInstance::SpawnedMonsterValue--;

												//}
											}
										
										}
									}

									if (UNovaGameInstance::MyRoomInfo->RoomOwner != UNovaGameInstance::IDcode) {
										// �̹� ������ ���� �̹Ƿ�, ���� ������ ������Ʈ ���ش�.
										for (int i = 0; i < UNovaGameInstance::MonsterList.Num(); i++) {
											if (UNovaGameInstance::MonsterList[i]->Monsterid == monster->player_name()) {
												FVector CharacterLocation;
												CharacterLocation.X = monster->monsterposition()->loc()->X();
												CharacterLocation.Y = monster->monsterposition()->loc()->Y();
												CharacterLocation.Z = monster->monsterposition()->loc()->Z();


												UNovaGameInstance::MonsterList[i]->MoveToLocation(CharacterLocation);

												UNovaGameInstance::MonsterList[i]->IsAttacking = monster->IsAttacking();
												UNovaGameInstance::MonsterList[i]->AttackingCountNumber = monster->AttackCountNumber();
												UNovaGameInstance::MonsterList[i]->MonsterCurrentHP = static_cast<float>(monster->health());
												UNovaGameInstance::MonsterList[i]->IsMonsterAlive = RecvMonsterIsAlive;
												UNovaGameInstance::MonsterList[i]->AnotherPlayerAttack();
												//UE_LOG(LogTemp, Warning, TEXT("NRO : MonsterHP : %f(%d)"), UNovaGameInstance::MonsterList[i]->MonsterCurrentHP, UNovaGameInstance::MonsterList[i]->Monsterid);

												//// ������ ���¸� ó���ϴ� �κ��� Ŭ�󿡼��� �Ѵ�. 
												////if (UNovaGameInstance::MonsterList[i]->MonsterCurrentHP <= 0.0f && UNovaGameInstance::MonsterList[i]->IsMonsterAlive == true) {
												//if (UNovaGameInstance::MonsterList[i]->IsMonsterAlive == true
												//	&& monster->IsAlive() == false
												//	&& UNovaGameInstance::MonsterList[i]->DiedAnim == false) {
												//	UNovaGameInstance::MonsterList[i]->IsMonsterAlive = false;
												//	UNovaGameInstance::MonsterList[i]->MonsterDead();
												//	UNovaGameInstance::MonsterList[i]->DiedAnim = true;

												//	//UNovaGameInstance::SpawnedMonsterValue--;

												//}

											}
										}
									}
								}
							});
									
						}
					}
						//


					// ���ӻ����ڵ� ������ ���� ���
					if (MsgType == object::Type_GameState)
					{
						auto state = static_cast<const object::GameState*>(objpacket->packet_type());

						AsyncTask(ENamedThreads::GameThread, [state]() {


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

								//TODO
								// �� ���� ���� �ٸ� ���¿� �ִٰ� �κ� ���� ����̹Ƿ�, ���¸� �ٽ� ���Ѵ�.
								if (isfisrtconnect == false && UNovaGameInstance::IsNowGamePlaying) {
									for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++) {
										// anotherUserid�� �κ�� ���Դٴ� ������ �޾Ҵµ�, ���� Ŭ���̾�Ʈ�� ����Ǿ��ִ� �÷������� ���� ����Ʈ��
										// �ش� ���̵� �ִٸ� �� ���̵�� ��������Ѵ�. ĳ���� ����
										if (UNovaGameInstance::PlayingGameUserList[i]->CharacterID == antIDcode)
										{
											UNovaGameInstance::PlayingGameUserList[i]->Destroy();
											UNovaGameInstance::PlayingGameUserList.RemoveAt(i);

										}
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
									for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++) {
										// anotherUserid�� �α׾ƿ��ߴٴ� ������ �޾Ҵµ�, ���� Ŭ���̾�Ʈ�� ����Ǿ��ִ� �÷������� ���� ����Ʈ��
										// �ش� ���̵� �ִٸ� �� ���̵�� ��������Ѵ�.
										if (UNovaGameInstance::PlayingGameUserList[i]->CharacterID == antIDcode)
										{
											UNovaGameInstance::PlayingGameUserList[i]->Destroy();
											UNovaGameInstance::PlayingGameUserList.RemoveAt(i);
										}
									}
								}


							}
						});
						
					}
					*/

				}
				

			}
			
		}
	}
	return 0;
}

NovaSocketListener* NovaSocketListener::RunSocketListening()
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new NovaSocketListener();
	}
	return Runnable;
}

void NovaSocketListener::Shutdown()
{
	bThreadRun = false;

	GLog->Log("NovaSocketListener::Shutdown()");

	if (Runnable)
	{
		delete Runnable;
		Runnable = nullptr;
	}
}



// �˷��� ������(12/29)
// 1. ���� ������ ������ �κ� -> �ذ�(?) -> ���� �ذ��ε�, �������� ��������, ��Ƽ�÷��̿��� �׽�Ʈ�غ���
// 2. ĳ���Ͱ� ���Ϳ��� ��ų ���ݽ� ��ųó���� �ȵ� -> �ȵɶ��� �ְ�.. �ɶ��� �ְ� .. ����� �ߵȴ�. 
// 3. ���Ͱ� �׾ ������� destory �ϴ� �κ� ���� -> �ذ�(���;��� ��ü���� �ı��ǰ� ��Ͽ��� ��������� �ߴ��� �ذ��)
// 4. ������ �˼����� ������ ���� ���� -> �ذ�(?) -> ���޹��� �����͸� ���ο� �迭�� �����ϰ�, �� �迭��ü�� �Ű������� AsynTask �־���. 
// �׸��� �ѱ� �����Լ����� �ؼ��ؼ� ó���ϵ��� �ٲ���� ������ ���� �ʴ´�.
//
// �ؾ��Ұ�
// 1. ĳ���� ����Ʈ�� ���������� �����Ұ� -> �ذ�
// 2. ��Ƽ�÷��� �׽�Ʈ -> ���� 2�� �׽�Ʈ��
// 3. ĳ���� ��Ʈ�ѷ��� AI�� �����غ���

// ������ 12/30
/*
1. �����÷��̿� ������ �ٸ������ ��ٸ��� ȭ���� �ȶ��. �׸��� �ð� ī��Ʈ�� �ȶ��.
-> �ð� ī��Ʈ�� Ŭ���̾�Ʈ���� ��ü������(?) �����ϴ°� ���?
2. ������ ������(���� Ŭ��� ���̴� ����� �ٸ��͵�)


�ؾ��Ұ�
1. �ȵ���̵� ���� �� �׽�Ʈ(�̰� �ȵǸ� �ռ��� ���� �� �ǹ̰� ����..)
2. Ŭ���̾�Ʈ 4�� �����ؼ� �׽�Ʈ�غ���(��Ʈ�� 2��, ����ũž 2��)


*/