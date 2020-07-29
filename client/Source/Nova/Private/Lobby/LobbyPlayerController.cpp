// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPlayerController.h"
#include "NovaGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.h"
#include "Kismet/GameplayStatics.h"
#include <string>
#if PLATFORM_WINDOWS
#include <Windows.h>
#endif



ALobbyPlayerController::ALobbyPlayerController()
{
	// 블루프린트로 제작된 블루프린트 위젯을 연결해준다.
	static ConstructorHelpers::FClassFinder<ULobbyWidget> UI_LOBBY_C(TEXT(
		"/Game/NovaProject/UI/BP_Lobby.BP_Lobby_C"));
	if (UI_LOBBY_C.Succeeded())
	{
		LobbyHUD = UI_LOBBY_C.Class->GetDefaultObject<ULobbyWidget>();
	}

	static ConstructorHelpers::FClassFinder<ULobbyWidget> MESSAGE_C(TEXT(
		"/Game/NovaProject/UI/BP_ChattingMessage.BP_ChattingMessage_C"));
	if (MESSAGE_C.Succeeded())
	{
		MessageWidgetClass = MESSAGE_C.Class;
	}
	

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_WAITING(TEXT(
		"/Game/NovaProject/UI/BP_Waiting.BP_Waiting_C"));
	if (UI_WAITING.Succeeded())
	{
		WaitingScreenClass = UI_WAITING.Class->GetDefaultObject<UUserWidget>();
	}
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 타이틀 인스턴스를 만들어서 위젯을 생성해주고 뷰포트에 띄워준다.
	LobbyHUDClass = CreateWidget<ULobbyWidget>(GetWorld(), LobbyHUD->GetClass());
	LobbyHUDClass->AddToViewport();

	MessageWidget = CreateWidget<ULobbyWidget>(this, MessageWidgetClass); 


	// 플레이어 컨트롤러를 만들어준다. 
	APlayerController* PController = GetWorld()->GetFirstPlayerController();
	PController->bShowMouseCursor = true; // 마우스 표시
	PController->bEnableClickEvents = true; // 클릭이벤트 가능
	PController->bEnableMouseOverEvents = true; // 마우스 오버 이벤트 가능

	// 게임 인스턴스 가져오기
	//UNovaGameInstance* gameInstance = Cast<UNovaGameInstance>(GetWorld()->GetGameInstance());
}



void ALobbyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!UNovaGameInstance::bIsConnected) return;

	if (UNovaGameInstance::IsNeedUpdateMessage)
	{
		UpdateNewMessage();
	}

	if (UNovaGameInstance::LobbyUserListUpdate)
	{
		UpdateUserList();
	}

	if (UNovaGameInstance::LobbyRoomListUpdate)
	{
		UpdateRoomList();
	}

}

bool ALobbyPlayerController::WriteMessageToChatWindow(const FString& msg)
{
	//UNovaGameInstance* gameInstance = Cast<UNovaGameInstance>(GetWorld()->GetGameInstance());

	int32 gamecode = 0; // 로비 0
	// FString -> string
	std::string userid = std::string(TCHAR_TO_UTF8(*UNovaGameInstance::GetUserID()));
	
	UE_LOG(LogTemp, Warning, TEXT("Write Meg : %s"), *msg);
	// 입력한 채팅 메세지를 서버에 보낸다.
	bool nSendnend = UNovaGameInstance::NovaSocketcli->SendMessageToServer(msg, gamecode, userid);;
	//UE_LOG(LogTemp, Warning, TEXT("ObjectPacket Send LPC : [ %d ] (1: Success!, 0: Faild!)"), nSendnend);

	//return nSendnend;
	return false;
}

void ALobbyPlayerController::UpdateNewMessage()
{
	ULobbyWidget* lbwg = Cast<ULobbyWidget>(LobbyHUDClass);
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

void ALobbyPlayerController::UpdateUserList()
{
	ULobbyWidget* lbwg = Cast<ULobbyWidget>(LobbyHUDClass);
	if (lbwg != nullptr)
	{
		// 우선 스크롤박스의 내용을 지워준다.
		lbwg->RemoveUserList();

		// 그리고 현재 가지고 있는 접속 유저들의 정보를 업데이트 해준다.
		for (int i = 0; i < UNovaGameInstance::PlayerList.Num(); i++) {
	
			//if (UNovaGameInstance::PlayerList[i]->NowGameStateCode == UNovaGameInstance::State::LOBBY)
			//{
				PlayerInfo* tmp = UNovaGameInstance::PlayerList[i];
				//UE_LOG(LogTemp, Warning, TEXT("Update User List3 : %s"), UNovaGameInstance::PlayerList[i]->Userid.c_str()); // 왜 데이터가 없다고 뜨냐!!!
				//UE_LOG(LogTemp, Warning, TEXT("Update User List2 : %s"), *tmp->Userid.c_str()); // 왜 데이터가 없다고 뜨냐!!!
				FString id = UTF8_TO_TCHAR(tmp->Userid.c_str());
				int32 cd = tmp->NowGameStateCode;
				int32 MP = tmp->MaxPoint;
				UE_LOG(LogTemp, Warning, TEXT("Update User List [ID : %s/ Code : %d / MP : %d]"), *id, tmp->IDcode, MP); // 잘된다
				lbwg->UpdateUserList(FText::FromString(id), cd, MP);

			//}
		}
	}
	UNovaGameInstance::LobbyUserListUpdate = false;
}


// 다른클라이언트로부터 룸이 생성되었다는 메세지를 받았을때 로비의 룸리스트를 업데이트 해주는 메소드
void ALobbyPlayerController::UpdateRoomList()
{
	ULobbyWidget* lbwg = Cast<ULobbyWidget>(LobbyHUDClass);
	if (lbwg != nullptr)
	{
		lbwg->RemoveRoomList();

		for (int i = 0; i < UNovaGameInstance::TempRoomInfo.Num(); i++)
		{
			lbwg->MakeRoomList(FText::FromString(UTF8_TO_TCHAR(UNovaGameInstance::TempRoomInfo[i]->RoomName.c_str()))
				, UNovaGameInstance::TempRoomInfo[i]->RoomNumber
				, UNovaGameInstance::TempRoomInfo[i]->CurrentUserNumber
				, UNovaGameInstance::TempRoomInfo[i]->MaxUserNumber
				, UNovaGameInstance::TempRoomInfo[i]->IsThisRoomPlayingGame
				, UNovaGameInstance::TempRoomInfo[i]->IsThisRoomLock);
			UE_LOG(LogTemp, Warning, TEXT("Update Room List"));
		}
	}

	UNovaGameInstance::LobbyRoomListUpdate = false;
}

void ALobbyPlayerController::EnterRoom(const int32& roomnumber)
{
	// 방에 입장하는 경우

	int32 EnterRoomNumber = roomnumber;

	flatbuffers::FlatBufferBuilder builder(1024); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateRoomInfo(builder, // 빌더에 원하는 데이터를 담는다. 여기에선 로그인데이터를 만든다.
		0, EnterRoomNumber); // 몇번방에 들어가는지에대한 데이터를 보내준다.
	auto pk = object::CreatePacket(builder, object::Type_RoomInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);


	if (nSendLend == true)
	{
		//UGameplayStatics::OpenLevel(GetWorld(), TEXT("Room"));
	}
}

bool ALobbyPlayerController::SendRoomInfo(const FText& roomName, const int32& MaxUserNumber, const FText& Password)
{
	// 방생성후 서버에 데이터를 보내는 함수.
	// 방번호를 -1로 보낸다. 최초의 방생성
	// 그리고 서버에서 중복되지 않은 방번호를 받아 방번호를 바꿔준다.
	//================================================================================
	// 전송 시작

	// FText 형식을 FString형식으로 변환
	FString RoomName = *roomName.ToString();
	FString password = *Password.ToString();
	int32 MaxNum = MaxUserNumber;

	flatbuffers::FlatBufferBuilder builder(1024); // 데이터를 담을 플랫버퍼빌더를 만든다.
	object::RoomInfoBuilder roombuilder(builder);
	roombuilder.add_RoomName(builder.CreateString(std::string(TCHAR_TO_UTF8(*RoomName))));
	roombuilder.add_MaxUserNumber(MaxNum);
	roombuilder.add_RoomNumber(-1); // 내가 방을 생성하는 경우 방번호를 -1로 보낸다.
	roombuilder.add_RoomOwner(UNovaGameInstance::IDcode);
	roombuilder.add_Password(builder.CreateString(std::string(TCHAR_TO_UTF8(*password))));
	if (std::string(TCHAR_TO_UTF8(*password)) == "") // 비밀번호가 적혀져 있으면 비공개방, 없으면 공개방
	{
		roombuilder.add_IsThisRoomLock(false);
	}
	else {
		roombuilder.add_IsThisRoomLock(true);
	}
	auto msg = roombuilder.Finish();
	auto pk = object::CreatePacket(builder, object::Type_RoomInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);


	UE_LOG(LogTemp, Warning, TEXT("Room Info Send : [ %d ] (1: true, 0: false)"), nSendLend);
	

	return true;

}

