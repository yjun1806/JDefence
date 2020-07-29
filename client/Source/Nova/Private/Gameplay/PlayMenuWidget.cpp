// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "NovaPlayerController.h"
#include "NovaGameInstance.h"
#include "Components/ProgressBar.h"





void UPlayMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));
	if (nullptr != ResumeButton)
	{
		// 해당 버튼이 클릭되었을때 OnResumClicked함수가 호출된다.
		ResumeButton->OnClicked.AddDynamic(this, &UPlayMenuWidget::OnResumeClicked);
	}

	ReturnToLobbyButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToLobby")));
	if (nullptr != ReturnToLobbyButton)
	{
		// 위와 비슷한 의미의 기능을 한다.
		ReturnToLobbyButton->OnClicked.AddDynamic(this, &UPlayMenuWidget::OnReturnToLobbyClicked);
	}

}

// 게임진행 버튼
void UPlayMenuWidget::OnResumeClicked()
{
	auto NovaPlayerController = Cast<ANovaPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != NovaPlayerController);

	RemoveFromParent(); // 떠있는 위젯을 지워주는 메소드
	NovaPlayerController->SetPause(false);
}

// 로비로 돌아가기 버튼
void UPlayMenuWidget::OnReturnToLobbyClicked()
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
	UNovaGameInstance::IsNowGamePlaying = false;
	UNovaGameInstance::NovaSocketcli->SendLeaveRoomInfo(UNovaGameInstance::NowPlayStateCode);
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lobby"));
}


