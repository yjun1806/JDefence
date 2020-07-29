// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomPlayerController.h"
#include "NovaGameInstance.h"
#include "RoomWidgetHUD.h"
#include "Kismet/GameplayStatics.h"




void ARoomPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (UNovaGameInstance::RoomUserListUpdate)
	{
		UpdateRoomUserList();
	}

	if (UNovaGameInstance::IsNeedUpdateMessage)
	{
		UpdateRoomChatMessage();
	}

	if (UNovaGameInstance::GameCounting)
	{
		StartCounting();
	}

}

void ARoomPlayerController::BeginPlay()
{
	RoomWidgetHUD = CreateWidget<URoomWidgetHUD>(GetWorld(), RoomWidgetClass->GetClass());
	RoomWidgetHUD->AddToViewport();

	LoadingScreen = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenClass->GetClass());


	// �÷��̾� ��Ʈ�ѷ��� ������ش�. 
	APlayerController* PController = GetWorld()->GetFirstPlayerController();
	PController->bShowMouseCursor = true; // ���콺 ǥ��
	PController->bEnableClickEvents = true; // Ŭ���̺�Ʈ ����
	PController->bEnableMouseOverEvents = true; // ���콺 ���� �̺�Ʈ ����


	URoomWidgetHUD* rwh = Cast<URoomWidgetHUD>(RoomWidgetHUD);
	if (rwh != nullptr)
	{
		rwh->GetGameData();
	}
	
}

ARoomPlayerController::ARoomPlayerController()
{
	static ConstructorHelpers::FClassFinder<URoomWidgetHUD> UI_ROOM_HUD(TEXT(
		"/Game/NovaProject/UI/BP_RoomHUD.BP_RoomHUD_C"));
	if (UI_ROOM_HUD.Succeeded())
	{
		RoomWidgetClass = UI_ROOM_HUD.Class->GetDefaultObject<URoomWidgetHUD>();
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_LOADING(TEXT(
		"/Game/NovaProject/UI/BP_LodingScreen.BP_LodingScreen_C"));
	if (UI_LOADING.Succeeded())
	{
		LoadingScreenClass = UI_LOADING.Class->GetDefaultObject<UUserWidget>();
	}
}

void ARoomPlayerController::UpdateRoomUserList()
{
	URoomWidgetHUD* rwh = Cast<URoomWidgetHUD>(RoomWidgetHUD);
	if (rwh != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("RoomUserListUpdate2 !!!!!!!!!!!! UserListNum : %d"), UNovaGameInstance::MyRoomInfo->user_list.Num());
		for (int i = 0; i < UNovaGameInstance::MyRoomInfo->user_list.Num(); i++)
		{
			//FString tmpstr = UTF8_TO_TCHAR(UNovaGameInstance::MyRoomInfo.user_list().Get(i).userid().c_str());
			//bool isready = UNovaGameInstance::MyRoomInfo.user_list().Get(i).isready();
			//rwh->UpdateRMUserList(FText::FromString(tmpstr), isready);
			rwh->UpdateRMUserList();
			UE_LOG(LogTemp, Warning, TEXT("RoomUserListUpdate !!!!!!!!!!!!"));
		}

	}

	UNovaGameInstance::RoomUserListUpdate = false;
}

void ARoomPlayerController::UpdateRoomChatMessage()
{
	URoomWidgetHUD* lbwg = Cast<URoomWidgetHUD>(RoomWidgetHUD);
	if (lbwg != nullptr)
	{
		// UTF8 �������� ���ڿ��� �ٲ���� �̻��� �ܰ� �ȶ��.
		FString msg = UTF8_TO_TCHAR(UNovaGameInstance::ReceiveChatMessage.c_str());
		FString id = UTF8_TO_TCHAR(UNovaGameInstance::ReceiveMsgUserID.c_str());
		UE_LOG(LogTemp, Warning, TEXT("UPdateChat"));
		// FString -> FText�� ��ȯ�Ͽ� ����.
		lbwg->UpdateRoomChatmsg(FText::FromString(id), FText::FromString(msg));
	}
	UNovaGameInstance::IsNeedUpdateMessage = false;
}

void ARoomPlayerController::StartCounting()
{
	URoomWidgetHUD* lbwg = Cast<URoomWidgetHUD>(RoomWidgetHUD);
	if (lbwg != nullptr)
	{
		lbwg->StartCounting();
	}
	if (UNovaGameInstance::CountingNumber == 0) {
		LoadingScreen->AddToViewport();

		UE_LOG(LogTemp, Warning, TEXT("2StartGame / PlayingGameUserNum %d"), UNovaGameInstance::PlayingGameUserList.Num());
		if (UNovaGameInstance::PlayingGameUserList.Num() > 0) {
			UE_LOG(LogTemp, Warning, TEXT("StartGame / PlayingGameUserNum %d"), UNovaGameInstance::PlayingGameUserList.Num());
			//for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++)
			//{
			//	//UNovaGameInstance::PlayingGameUserList[i]->Destroy();
			//}
			UNovaGameInstance::PlayingGameUserList.Empty();
		}
		UNovaGameInstance::TempPlayerData.player_name = -1; // ù �ʱ�ȭ
		UNovaGameInstance::MonsterList.Empty();
		UNovaGameInstance::CurrentPoint = 0;
		//UNovaGameInstance::CountingNumber = -1;
		//UGameplayStatics::OpenLevel(GetWorld(), TEXT("PlayLevel"));
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("CoopLandscape_Map"));
		
		//UNovaGameInstance::NovaSocketcli->SendGameStartInfo(UNovaGameInstance::NowPlayStateCode, true);
	}

	UNovaGameInstance::GameCounting = false;
}

bool ARoomPlayerController::Room_WriteMessageToChatWindow(const FString& msg)
{
	int32 gamecode = UNovaGameInstance::NowPlayStateCode;
	// FString -> string
	std::string userid = std::string(TCHAR_TO_UTF8(*UNovaGameInstance::GetUserID()));

	// �Է��� ä�� �޼����� ������ ������.
	bool nSendnend = UNovaGameInstance::NovaSocketcli->SendMessageToServer(msg, gamecode, userid);;

	return nSendnend;
}
