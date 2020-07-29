// Fill out your copyright notice in the Description page of Project Settings.

#include "GameEndWidget.h"
#include "Components/Button.h"
#include "NovaGameInstance.h"
#include "TimerManager.h"




void UGameEndWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BackToLobbyBtn = Cast<UButton>(GetWidgetFromName(TEXT("GameEnd_BackToLobbybtn")));
	BackToLobbyBtnWin = Cast<UButton>(GetWidgetFromName(TEXT("GameWin_BackToLobbybtn")));
	if (BackToLobbyBtn != nullptr) {
		BackToLobbyBtn->OnClicked.AddDynamic(this, &UGameEndWidget::BackToLobbyFtn);
	}

	if (BackToLobbyBtnWin != nullptr) {
		BackToLobbyBtnWin->OnClicked.AddDynamic(this, &UGameEndWidget::BackToLobbyFtn);
	}
}

void UGameEndWidget::BackToLobbyFtn()
{
	//for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++)
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("BackToLobby! / PlayingGameUserNum %d"), UNovaGameInstance::PlayingGameUserList.Num());
	//	UNovaGameInstance::PlayingGameUserList[i]->Destroy();
	//}
	/*for (int i = 0; i < UNovaGameInstance::MonsterList.Num(); i++)
	{
		UNovaGameInstance::MonsterList[i]->Destroy();
	}*/
	UNovaGameInstance::MonsterList.Empty();

	UNovaGameInstance::PlayingGameUserList.Empty();
	UNovaGameInstance::NovaSocketcli->SendLeaveRoomInfo(UNovaGameInstance::NowPlayStateCode);
	UNovaGameInstance::NovaSocketcli->SendRefreshItemData();

	// 게임종료시 몬스터 체크 타이머 삭제
	auto GI = Cast<UNovaGameInstance>(GetGameInstance());
	GI->ClearCheackMonsterTimer();

	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lobby"));
}
