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

					// 받은 데이터를 버퍼에 담는다.
					//uint8_t* rbuffer = ReceivedData.GetData();

					TArray<uint8> CopyBuffer;
					CopyBuffer.Append(ReceivedData);
					
					//auto objpacket = object::GetPacket(ReceivedData.GetData()); // 서버에서 받은 데이터 비직렬화
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

					


					//// 테스트
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
							});
						}
							break;

							//===============================================================================================
							//===============================================================================================
							//===============================================================================================

						case object::Type_AllUser://===============================================================================================
						{
							auto alu = static_cast<const object::AllUser*>(objpacket->packet_type());
							// 우선 현재 내가 가지고 있는 유저리스트에 대한 정보를 업데이트 하기 위해 비워준다.
							AsyncTask(ENamedThreads::GameThread, [alu]() {

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

								// 게임방 혹은 게임플레이를 시작하기 전
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
								// 로비 플레이어 컨트롤러가 틱동작부분에서 메소드를 실행시킬 수 있도록 true값으로 바꿔준다.
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

								// 내 정보를 업데이트 해야 하는 부분은 자신의 체력(왜냐면 서버에서 변경해주므로, 살았는지 죽었는지 등)
								if (UNovaGameInstance::IDcode == characterid)
								{
									//UE_LOG(LogTemp, Warning, TEXT("MyData %d"), UNovaGameInstance::IDcode);

									UNovaGameInstance::MyCharacterData->CharacterCurrentHP = static_cast<float>(characterHP);
									UNovaGameInstance::CurrentPoint = totalP;
									

								}
								else // 받은 데이터가 내 정보가 아닌 경우, 다른 유저들의 정보를 전달받은 것이다.
								{
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
									// 전역배열에 저장
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
							// 몬스터 스폰 데이터를 받았을때 몬스터를 스폰시켜주는 부분.
							auto spawndata = static_cast<const object::SpawnMonsterData*> (objpacket->packet_type());
							
								AsyncTask(ENamedThreads::GameThread, [spawndata]() {
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
								});
							

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

							AsyncTask(ENamedThreads::GameThread, [monster]() {

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

								std::string msg = sysmsg->message()->str(); // TODO : str로 하면 뭐가다를까?
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
					if (UNovaGameInstance::NowPlayStateCode == 0) { // 로비 0

						//// 비공개방에 입장하는 경우, 입력한 패스워드 서버에 전송 -> (여기) 서버에서 비밀번호가 일치하는지 체크 후 리턴
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


						//랭킹보기 버튼을 눌러서 서버에서 데이터를 받은 경우
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
									// 전역배열에 저장
									UNovaGameInstance::TopRanker.Add(temp);
								}
								UNovaGameInstance::UpdateTopRanking = true;
							});

						}


						//유저리스트에 대한 정보가 들어있다면
						else if (MsgType == object::Type_AllUser)
						{
							auto alu = static_cast<const object::AllUser*>(objpacket->packet_type());
							// 우선 현재 내가 가지고 있는 유저리스트에 대한 정보를 업데이트 하기 위해 비워준다.
							AsyncTask(ENamedThreads::GameThread, [alu]() {

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
							});
						}

						// 채팅메세지를 받은 경우
						else if (MsgType == object::Type_ChatMessage) {
							auto chat = static_cast<const object::ChatMessage*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [chat]() {

								std::string msg = chat->ChatMsg()->c_str();
								std::string id = chat->SenderID()->c_str();
								//UE_LOG(LogTemp, Warning, TEXT("Write Meg : %s : %s"), UTF8_TO_TCHAR(id.c_str()), UTF8_TO_TCHAR(msg.c_str()));

								UNovaGameInstance::ReceiveChatMessage = msg;
								UNovaGameInstance::ReceiveMsgUserID = id;
								// 로비 플레이어 컨트롤러가 틱동작부분에서 메소드를 실행시킬 수 있도록 true값으로 바꿔준다.
								UNovaGameInstance::IsNeedUpdateMessage = true;

								//lobbywidget->ChatNewMessage(UTF8_TO_TCHAR(msg.c_str()));
							});

						}

						else if (MsgType == object::Type_RoomInfo)
						{
							auto ri = static_cast<const object::RoomInfo*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [ri]() {

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
						//		// 게임 결과정보를 받는 경우
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
						//		// 게임방에 참여중인데, 게임스타트 정보에 대한 메세지를 받는 경우
						//		// 즉, 이 경우는 방장이 게임 시작 버튼을 눌러서 게임을 시작하려는 경우이다.
						else if (MsgType == object::Type_GameStartInfo)
						{
							auto str = static_cast<const object::GameStartInfo*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [str]() {

								// 게임방 혹은 게임플레이를 시작하기 전
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

							});
							
						}
						//
						//		// 채팅메세지를 받은 경우
						else if (MsgType == object::Type_ChatMessage) {
							auto chat = static_cast<const object::ChatMessage*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [chat]() {

								std::string msg = chat->ChatMsg()->c_str();
								std::string id = chat->SenderID()->c_str();
								//UE_LOG(LogTemp, Warning, TEXT("Write Meg : %s : %s"), UTF8_TO_TCHAR(id.c_str()), UTF8_TO_TCHAR(msg.c_str()));

								UNovaGameInstance::ReceiveChatMessage = msg;
								UNovaGameInstance::ReceiveMsgUserID = id;
								// 로비 플레이어 컨트롤러가 틱동작부분에서 메소드를 실행시킬 수 있도록 true값으로 바꿔준다.
								UNovaGameInstance::IsNeedUpdateMessage = true;

								//lobbywidget->ChatNewMessage(UTF8_TO_TCHAR(msg.c_str()));
							});
						
						}
						
						// 시스템 메세지를 받는 경우
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
						
						// 게임시간메세지를 받는 경우
						else if (MsgType == object::Type_SystemTime) {
							auto systime = static_cast<const object::SystemTime*>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [systime]() {

								UNovaGameInstance::GameTime = systime->Time();
								UNovaGameInstance::UpdateGameTime = true;
								UE_LOG(LogTemp, Warning, TEXT("LeftGameTime : %d"), UNovaGameInstance::GameTime);
							});
						}
						//

						// 캐릭터 데이터를 받는 경우
						else if (MsgType == object::Type_PlayerData)
						{
							auto player = static_cast<const object::PlayerData*> (objpacket->packet_type());
							//auto player = Cast<const object::PlayerData>(objpacket->packet_type());

							AsyncTask(ENamedThreads::GameThread, [player]() {

								//UE_LOG(LogTemp, Warning, TEXT("*----------- | HasPlayerData"));
								//UE_LOG(LogTemp, Warning, TEXT("Maybe Here!! %s  / %d"), UTF8_TO_TCHAR(objpacket->playerdata().player_name().c_str()), objpacket->playerdata().health());


								// 내 정보를 업데이트 해야 하는 부분은 자신의 체력(왜냐면 서버에서 변경해주므로, 살았는지 죽었는지 등)
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
								// 최초의 데이터는 배열에 추가시키고, 그후의 데이터는 업데이트 시킨다.
								bool CharacterSpawn = false;
								UE_LOG(LogTemp, Warning, TEXT("False?!!!!!! %d"), CharacterSpawn);

								for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++)
								{
									// 배열 내부에 데이터를 받은 캐릭터 이름과 같은 캐릭터가 존재하는 경우
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
									// 아직 내 화면에 데이터를 받은 캐릭터를 스폰시켜주지 않은 경우이다.
									// 스위치를 true로 바꿔준뒤 캐릭터를 스폰시켜준다.
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



									// 이미 스폰된 캐릭터 이므로, 캐릭터의 정보만 업데이트 해준다.
									//UNovaGameInstance::TempPlayerData = objpacket->playerdata();
									// 이 부분은 자신의 정보는 업데이트 할 필요가 없다. 
									for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++) {
										// 부활한 경우
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

											UNovaGameInstance::PlayingGameUserList[i]->IsAttacking = player->IsAttacking(); // 공격중인가?
											UNovaGameInstance::PlayingGameUserList[i]->AttackingCountNumber = player->AttackCountNumber(); // 카운트번호 무엇인가?
											UNovaGameInstance::PlayingGameUserList[i]->CharacterCurrentHP = static_cast<float>(player->health()); // 체력은 몇인가?
											UNovaGameInstance::PlayingGameUserList[i]->IsPlayerAlive = player->IsAlive(); // 살아있는가?
											UNovaGameInstance::PlayingGameUserList[i]->IsSkillUsing = player->IsSkillUsing();
											UNovaGameInstance::PlayingGameUserList[i]->SkillUsingNumber = player->SkillNumber();
											UNovaGameInstance::PlayingGameUserList[i]->AnotherPlayerAttack();//다른플레이어가 공격하면 발동
											UNovaGameInstance::PlayingGameUserList[i]->AnotherPlayer_SkillUsing();//다른플레이어가 스킬사용시 발동

											//UNovaGameInstance::PlayingGameUserList[i]->UpdateCharacterHPbar();//다른플레이어 체력업데이트

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


								// 최초의 데이터는 배열에 추가시키고, 그후의 데이터는 업데이트 시킨다.
								bool isFirstUpdate = false;
								for (int i = 0; i < UNovaGameInstance::MonsterList.Num(); i++)
								{
									// 배열 내부에 데이터를 받은 이름과 같은 몬스터가 존재하는 경우
									if (UNovaGameInstance::MonsterList[i]->Monsterid == RecvMonsterID)
									{
										isFirstUpdate = true;
										break;
									}
								}


								if (isFirstUpdate == false)
								{
									// 아직 내 화면에 데이터를 받은 몬스터를 스폰시켜주지 않은 경우이다.
									// 스위치를 true로 바꿔준뒤 몬스터를 스폰시켜준다.
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
										// 내가 방장인 경우, 몬스터 데이터를 받을때 체력부분만 받는다.
										for (int i = 0; i < UNovaGameInstance::MonsterList.Num(); i++) {

											if (UNovaGameInstance::MonsterList[i]->Monsterid == monster->player_name()) {
												UNovaGameInstance::MonsterList[i]->MonsterCurrentHP = static_cast<float>(monster->health());
												UNovaGameInstance::MonsterList[i]->IsMonsterAlive = RecvMonsterIsAlive;
											//UE_LOG(LogTemp, Warning, TEXT("RO : MonsterHP : %d(%d)"), monster->IsAlive(), UNovaGameInstance::MonsterList[i]->Monsterid);
												// 만약 현재 체력이 0인데, 살아있는 상태라면 죽었다는 상태로 바꿔주고, 시체모션으로 변화시켜준다.
												//if (UNovaGameInstance::MonsterList[i]->MonsterCurrentHP <= 0.0f && UNovaGameInstance::MonsterList[i]->IsMonsterAlive == true) {
														
												//UNovaGameInstance::MonsterList[i]->IsMonsterAlive = false; // 몬스터가 죽었다는 상태로 바꿔준다.
												//UNovaGameInstance::MonsterList[i]->MonsterDead();
												//UNovaGameInstance::MonsterList[i]->DiedAnim = true;


													//UNovaGameInstance::SpawnedMonsterValue--;

												//}
											}
										
										}
									}

									if (UNovaGameInstance::MyRoomInfo->RoomOwner != UNovaGameInstance::IDcode) {
										// 이미 스폰된 몬스터 이므로, 몬스터 정보만 업데이트 해준다.
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

												//// 몬스터의 상태를 처리하는 부분은 클라에서만 한다. 
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


					// 게임상태코드 정보를 받은 경우
					if (MsgType == object::Type_GameState)
					{
						auto state = static_cast<const object::GameState*>(objpacket->packet_type());

						AsyncTask(ENamedThreads::GameThread, [state]() {


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

								//TODO
								// 그 외의 경우는 다른 상태에 있다가 로비에 들어온 경우이므로, 상태를 다시 비교한다.
								if (isfisrtconnect == false && UNovaGameInstance::IsNowGamePlaying) {
									for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++) {
										// anotherUserid가 로비로 들어왔다는 정보를 받았는데, 나의 클라이언트에 저장되어있는 플레이중인 유저 리스트에
										// 해당 아이디가 있다면 그 아이디는 지워줘야한다. 캐릭터 제거
										if (UNovaGameInstance::PlayingGameUserList[i]->CharacterID == antIDcode)
										{
											UNovaGameInstance::PlayingGameUserList[i]->Destroy();
											UNovaGameInstance::PlayingGameUserList.RemoveAt(i);

										}
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
									for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++) {
										// anotherUserid가 로그아웃했다는 정보를 받았는데, 나의 클라이언트에 저장되어있는 플레이중인 유저 리스트에
										// 해당 아이디가 있다면 그 아이디는 지워줘야한다.
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



// 알려진 문제점(12/29)
// 1. 몬스터 스폰시 씹히는 부분 -> 해결(?) -> 거의 해결인듯, 아직까진 문제없음, 멀티플레이에서 테스트해볼것
// 2. 캐릭터가 몬스터에게 스킬 공격시 스킬처리가 안됨 -> 안될때도 있고.. 될때도 있고 .. 현재는 잘된다. 
// 3. 몬스터가 죽어서 사라질때 destory 하는 부분 에러 -> 해결(몬스터액터 자체에서 파괴되고 목록에서 사라지도록 했더니 해결됨)
// 4. 이유를 알수없는 엑세스 위반 에러 -> 해결(?) -> 전달받은 데이터를 새로운 배열에 복사하고, 그 배열자체를 매개변수로 AsynTask 넣었다. 
// 그리고 넘긴 람다함수에서 해석해서 처리하도록 바꿨더니 에러가 나지 않는다.
//
// 해야할것
// 1. 캐릭터 리스트도 맵형식으로 변경할것 -> 해결
// 2. 멀티플레이 테스트 -> 현재 2명 테스트중
// 3. 캐릭터 컨트롤러를 AI로 변경해볼것

// 문제점 12/30
/*
1. 게임플레이에 들어갔을때 다른사람을 기다리는 화면이 안뜬다. 그리고 시간 카운트도 안뜬다.
-> 시간 카운트는 클라이언트에서 자체적으로(?) 실행하는게 어떨까?
2. 몬스터의 딜레이(방장 클라와 보이는 모습이 다른것등)


해야할것
1. 안드로이드 포팅 후 테스트(이게 안되면 앞서한 모든게 다 의미가 없다..)
2. 클라이언트 4개 접속해서 테스트해보기(노트북 2개, 데스크탑 2개)


*/