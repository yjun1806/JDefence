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
		// �ش� ��ư�� Ŭ���Ǿ����� OnResumClicked�Լ��� ȣ��ȴ�.
		ResumeButton->OnClicked.AddDynamic(this, &UPlayMenuWidget::OnResumeClicked);
	}

	ReturnToLobbyButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToLobby")));
	if (nullptr != ReturnToLobbyButton)
	{
		// ���� ����� �ǹ��� ����� �Ѵ�.
		ReturnToLobbyButton->OnClicked.AddDynamic(this, &UPlayMenuWidget::OnReturnToLobbyClicked);
	}

}

// �������� ��ư
void UPlayMenuWidget::OnResumeClicked()
{
	auto NovaPlayerController = Cast<ANovaPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != NovaPlayerController);

	RemoveFromParent(); // ���ִ� ������ �����ִ� �޼ҵ�
	NovaPlayerController->SetPause(false);
}

// �κ�� ���ư��� ��ư
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


