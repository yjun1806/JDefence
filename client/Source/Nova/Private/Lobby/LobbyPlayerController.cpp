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
	// �������Ʈ�� ���۵� �������Ʈ ������ �������ش�.
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

	// Ÿ��Ʋ �ν��Ͻ��� ���� ������ �������ְ� ����Ʈ�� ����ش�.
	LobbyHUDClass = CreateWidget<ULobbyWidget>(GetWorld(), LobbyHUD->GetClass());
	LobbyHUDClass->AddToViewport();

	MessageWidget = CreateWidget<ULobbyWidget>(this, MessageWidgetClass); 


	// �÷��̾� ��Ʈ�ѷ��� ������ش�. 
	APlayerController* PController = GetWorld()->GetFirstPlayerController();
	PController->bShowMouseCursor = true; // ���콺 ǥ��
	PController->bEnableClickEvents = true; // Ŭ���̺�Ʈ ����
	PController->bEnableMouseOverEvents = true; // ���콺 ���� �̺�Ʈ ����

	// ���� �ν��Ͻ� ��������
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

	int32 gamecode = 0; // �κ� 0
	// FString -> string
	std::string userid = std::string(TCHAR_TO_UTF8(*UNovaGameInstance::GetUserID()));
	
	UE_LOG(LogTemp, Warning, TEXT("Write Meg : %s"), *msg);
	// �Է��� ä�� �޼����� ������ ������.
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
		// UTF8 �������� ���ڿ��� �ٲ���� �̻��� �ܰ� �ȶ��.
		FString msg = UTF8_TO_TCHAR(UNovaGameInstance::ReceiveChatMessage.c_str());
		FString id = UTF8_TO_TCHAR(UNovaGameInstance::ReceiveMsgUserID.c_str());
		UE_LOG(LogTemp, Warning, TEXT("UPdateChat"));
		// FString -> FText�� ��ȯ�Ͽ� ����.
		lbwg->UpdateChatmsg(FText::FromString(id), FText::FromString(msg));
	}
	UNovaGameInstance::IsNeedUpdateMessage = false;
}

void ALobbyPlayerController::UpdateUserList()
{
	ULobbyWidget* lbwg = Cast<ULobbyWidget>(LobbyHUDClass);
	if (lbwg != nullptr)
	{
		// �켱 ��ũ�ѹڽ��� ������ �����ش�.
		lbwg->RemoveUserList();

		// �׸��� ���� ������ �ִ� ���� �������� ������ ������Ʈ ���ش�.
		for (int i = 0; i < UNovaGameInstance::PlayerList.Num(); i++) {
	
			//if (UNovaGameInstance::PlayerList[i]->NowGameStateCode == UNovaGameInstance::State::LOBBY)
			//{
				PlayerInfo* tmp = UNovaGameInstance::PlayerList[i];
				//UE_LOG(LogTemp, Warning, TEXT("Update User List3 : %s"), UNovaGameInstance::PlayerList[i]->Userid.c_str()); // �� �����Ͱ� ���ٰ� �߳�!!!
				//UE_LOG(LogTemp, Warning, TEXT("Update User List2 : %s"), *tmp->Userid.c_str()); // �� �����Ͱ� ���ٰ� �߳�!!!
				FString id = UTF8_TO_TCHAR(tmp->Userid.c_str());
				int32 cd = tmp->NowGameStateCode;
				int32 MP = tmp->MaxPoint;
				UE_LOG(LogTemp, Warning, TEXT("Update User List [ID : %s/ Code : %d / MP : %d]"), *id, tmp->IDcode, MP); // �ߵȴ�
				lbwg->UpdateUserList(FText::FromString(id), cd, MP);

			//}
		}
	}
	UNovaGameInstance::LobbyUserListUpdate = false;
}


// �ٸ�Ŭ���̾�Ʈ�κ��� ���� �����Ǿ��ٴ� �޼����� �޾����� �κ��� �븮��Ʈ�� ������Ʈ ���ִ� �޼ҵ�
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
	// �濡 �����ϴ� ���

	int32 EnterRoomNumber = roomnumber;

	flatbuffers::FlatBufferBuilder builder(1024); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateRoomInfo(builder, // ������ ���ϴ� �����͸� ��´�. ���⿡�� �α��ε����͸� �����.
		0, EnterRoomNumber); // ����濡 ������������ �����͸� �����ش�.
	auto pk = object::CreatePacket(builder, object::Type_RoomInfo, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);


	if (nSendLend == true)
	{
		//UGameplayStatics::OpenLevel(GetWorld(), TEXT("Room"));
	}
}

bool ALobbyPlayerController::SendRoomInfo(const FText& roomName, const int32& MaxUserNumber, const FText& Password)
{
	// ������� ������ �����͸� ������ �Լ�.
	// ���ȣ�� -1�� ������. ������ �����
	// �׸��� �������� �ߺ����� ���� ���ȣ�� �޾� ���ȣ�� �ٲ��ش�.
	//================================================================================
	// ���� ����

	// FText ������ FString�������� ��ȯ
	FString RoomName = *roomName.ToString();
	FString password = *Password.ToString();
	int32 MaxNum = MaxUserNumber;

	flatbuffers::FlatBufferBuilder builder(1024); // �����͸� ���� �÷����ۺ����� �����.
	object::RoomInfoBuilder roombuilder(builder);
	roombuilder.add_RoomName(builder.CreateString(std::string(TCHAR_TO_UTF8(*RoomName))));
	roombuilder.add_MaxUserNumber(MaxNum);
	roombuilder.add_RoomNumber(-1); // ���� ���� �����ϴ� ��� ���ȣ�� -1�� ������.
	roombuilder.add_RoomOwner(UNovaGameInstance::IDcode);
	roombuilder.add_Password(builder.CreateString(std::string(TCHAR_TO_UTF8(*password))));
	if (std::string(TCHAR_TO_UTF8(*password)) == "") // ��й�ȣ�� ������ ������ �������, ������ ������
	{
		roombuilder.add_IsThisRoomLock(false);
	}
	else {
		roombuilder.add_IsThisRoomLock(true);
	}
	auto msg = roombuilder.Finish();
	auto pk = object::CreatePacket(builder, object::Type_RoomInfo, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);


	UE_LOG(LogTemp, Warning, TEXT("Room Info Send : [ %d ] (1: true, 0: false)"), nSendLend);
	

	return true;

}

