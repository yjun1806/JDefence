// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomWidgetHUD.h"
#include "LobbyWidget.h"
#include "NovaGameInstance.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BackgroundBlur.h"
#include "Components/Image.h"





void URoomWidgetHUD::NativeConstruct()
{
	// 방 번호랑 방 이름을 설정해주는 부분이다.
	RoomName = Cast<UTextBlock>(GetWidgetFromName(TEXT("rw_RoomName")));
	RoomNumber = Cast<UTextBlock>(GetWidgetFromName(TEXT("rw_RoomNumber")));
	int32 tempint = UNovaGameInstance::MyRoomInfo->RoomNumber;
	FString fint = FString::FromInt(tempint);
	std::string tempstr = "[ " + UNovaGameInstance::MyRoomInfo->RoomName + " ]";
	RoomName->SetText(FText::FromString(UTF8_TO_TCHAR(tempstr.c_str())));
	RoomNumber->SetText(FText::FromString(fint));

	LockImg = Cast<UImage>(GetWidgetFromName(TEXT("Lock")));
	if (UNovaGameInstance::MyRoomInfo->IsThisRoomLock) {
		LockImg->SetIsEnabled(true);
	}

	//채팅창 유저 이름 설정
	RoomChatUserId = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Room_UserID")));
	RoomChatUserId->SetText(FText::FromString(UTF8_TO_TCHAR(UNovaGameInstance::userLoginID.c_str())));

	UserIDText.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Room_User_1"))));
	UserIDText.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Room_User_2"))));
	UserIDText.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Room_User_3"))));
	UserIDText.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Room_User_4"))));

	UserSlot.Add(Cast<UBorder>(GetWidgetFromName(TEXT("Room_User_Slot_2"))));
	UserSlot.Add(Cast<UBorder>(GetWidgetFromName(TEXT("Room_User_Slot_3"))));
	UserSlot.Add(Cast<UBorder>(GetWidgetFromName(TEXT("Room_User_Slot_4"))));

	ReadySlot.Add(Cast<UBorder>(GetWidgetFromName(TEXT("Room_User_Readybtn_1"))));
	ReadySlot.Add(Cast<UBorder>(GetWidgetFromName(TEXT("Room_User_Readybtn_2"))));
	ReadySlot.Add(Cast<UBorder>(GetWidgetFromName(TEXT("Room_User_Readybtn_3"))));

	UserMP.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("User1_MP"))));
	UserMP.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("User2_MP"))));
	UserMP.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("User3_MP"))));
	UserMP.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("User4_MP"))));

	Readybtn = Cast<UButton>(GetWidgetFromName(TEXT("Room_GameReady")));
	ReadyCancelbtn = Cast<UButton>(GetWidgetFromName(TEXT("Room_GameReadyCancel")));
	Startbtn = Cast<UButton>(GetWidgetFromName(TEXT("Room_GameStart")));
	LeaveRoomToLobby = Cast<UButton>(GetWidgetFromName(TEXT("Room_GotoLobby")));

	CountingPanel = Cast<UBorder>(GetWidgetFromName(TEXT("Room_GameStartPanel")));
	CountingText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Room_CountingNumber")));

	if (UNovaGameInstance::MyRoomInfo->RoomOwner != UNovaGameInstance::IDcode)
	{
		Startbtn->SetVisibility(ESlateVisibility::Collapsed);
		Readybtn->SetVisibility(ESlateVisibility::Visible);
	}

	if (Readybtn != nullptr)
	{
		Readybtn->OnClicked.AddDynamic(this, &URoomWidgetHUD::GameReady);
	}

	if (ReadyCancelbtn != nullptr)
	{
		ReadyCancelbtn->OnClicked.AddDynamic(this, &URoomWidgetHUD::GameReadyCancel);
	}

	if (LeaveRoomToLobby != nullptr)
	{
		LeaveRoomToLobby->OnClicked.AddDynamic(this, &URoomWidgetHUD::LeaveGameRoom);
	}

	if (Startbtn != nullptr)
	{
		Startbtn->OnClicked.AddDynamic(this, &URoomWidgetHUD::StartGamePlay);
	}
	

	for (int i = 0; i < UNovaGameInstance::MyRoomInfo->MaxUserNumber-1; i++)
	{
		UserSlot[i]->SetVisibility(ESlateVisibility::Visible);
	}
}


// 참여한 유저 정보 띄워주는 부분
void URoomWidgetHUD::UpdateRMUserList()
{
	GLog->Log("RoomWidgetHUD::UserListUpdate()");
	int32 checkreadynumber = 1; // 왜냐면 방장 혼자 있다면 게임 시작을 바로 할수있으므로 1부터 시작

	//// 인원수에 따른 유저칸을 활성화해준다.
	for (int i = 0; i < UNovaGameInstance::MyRoomInfo->MaxUserNumber - 1; i++)
	{
		UserIDText[i+1]->SetText(FText::FromString(TEXT("")));
		UserSlot[i]->SetIsEnabled(false);
		UserMP[i]->SetText(FText::FromString(TEXT("0")));

	}

	// 유저들 리스트에서 데이터를 가져와서 위젯에 추가해주는 부분
	for (int i = 0; i < UNovaGameInstance::MyRoomInfo->user_list.Num(); i++)
	{
		// 방장인 경우 1번칸에 넣어준다.
		if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::MyRoomInfo->user_list[i]->useridCode)
		{
			FString tmpstr = UTF8_TO_TCHAR(UNovaGameInstance::MyRoomInfo->user_list[i]->userid.c_str());
			UserIDText[0]->SetText(FText::FromString(tmpstr));

			int32 tmpMP = UNovaGameInstance::MyRoomInfo->user_list[i]->MaxPoint;
			FString strMP = FString::FromInt(tmpMP);
			UserMP[0]->SetText(FText::FromString(strMP));

		}
		else 
		{
			if (i > 0)
			{
				FString tmpstr = UTF8_TO_TCHAR(UNovaGameInstance::MyRoomInfo->user_list[i]->userid.c_str());
				UserIDText[i]->SetText(FText::FromString(tmpstr));
				UserSlot[i - 1]->SetIsEnabled(true);

				int32 tmpMP = UNovaGameInstance::MyRoomInfo->user_list[i]->MaxPoint;
				FString strMP = FString::FromInt(tmpMP);
				UserMP[i]->SetText(FText::FromString(strMP));

				if(UNovaGameInstance::MyRoomInfo->user_list[i]->isready == true) {
					ReadySlot[i - 1]->SetIsEnabled(true);
					checkreadynumber++;
				}
				else {
					ReadySlot[i - 1]->SetIsEnabled(false);
					checkreadynumber--;
				}
			}
		}
	}

	if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
	{
		ABLOG(Error, TEXT("Im Owner!!"));
		Startbtn->SetVisibility(ESlateVisibility::Visible);
		Readybtn->SetVisibility(ESlateVisibility::Collapsed);
		ReadyCancelbtn->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (UNovaGameInstance::MyRoomInfo->user_list.Num() == checkreadynumber)
	{
		Startbtn->SetIsEnabled(true);
	}
	else
	{
		Startbtn->SetIsEnabled(false);

	}
}

void URoomWidgetHUD::GameReady()
{
	// 게임 준비했다는 메세지를 서버에 보냄
	Readybtn->SetVisibility(ESlateVisibility::Collapsed);
	ReadyCancelbtn->SetVisibility(ESlateVisibility::Visible);
	
	UNovaGameInstance::NovaSocketcli->SendGameReadyInfo(UNovaGameInstance::NowPlayStateCode, true);
}

void URoomWidgetHUD::GameReadyCancel()
{
	// 게임 준비 취소 메세지를 서버에 보냄
	ReadyCancelbtn->SetVisibility(ESlateVisibility::Collapsed);
	Readybtn->SetVisibility(ESlateVisibility::Visible);
	UNovaGameInstance::NovaSocketcli->SendGameReadyInfo(UNovaGameInstance::NowPlayStateCode, false);

}

void URoomWidgetHUD::LeaveGameRoom()
{
	UNovaGameInstance::NovaSocketcli->SendLeaveRoomInfo(UNovaGameInstance::NowPlayStateCode);
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lobby"));
}

void URoomWidgetHUD::StartGamePlay()
{
	UNovaGameInstance::NovaSocketcli->SendGameStartInfo(UNovaGameInstance::NowPlayStateCode, false);
}

void URoomWidgetHUD::StartCounting()
{
	if (!CountingPanel->IsVisible()) {
		CountingPanel->SetVisibility(ESlateVisibility::Visible);
	}
	FString tempstr = FString::FromInt(UNovaGameInstance::CountingNumber);
	CountingText->SetText(FText::FromString(tempstr));

	
}
