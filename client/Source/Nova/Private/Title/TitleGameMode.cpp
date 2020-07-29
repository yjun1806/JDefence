// Fill out your copyright notice in the Description page of Project Settings.

#include "TitleGameMode.h"
#include "TitleUserWidget.h"
#include "NovaSocketClient.h"
#include "Blueprint/UserWidget.h"
#include "NovaGameInstance.h"
#include "NovaSocketListener.h"




ATitleGameMode::ATitleGameMode()
{
	//
	// �������Ʈ�� ���۵� �������Ʈ ������ �������ش�.
	static ConstructorHelpers::FClassFinder<UTitleUserWidget> UI_TITLE_C(TEXT(
		"/Game/NovaProject/UI/BP_Title.BP_Title_C"));
	if (UI_TITLE_C.Succeeded())
	{
		TitleWidgetClass = UI_TITLE_C.Class->GetDefaultObject<UTitleUserWidget>();
	}

	static ConstructorHelpers::FClassFinder<APlayerController> BP_TITLE_PC(TEXT(
		"/Game/NovaProject/Blueprints/BP_TitlePC.BP_TitlePC_C"));

	if (BP_TITLE_PC.Class != NULL)
	{
		PlayerControllerClass = BP_TITLE_PC.Class;
	}

}

void ATitleGameMode::BeginPlay()
{
	// ���Ӹ�忡�� ����� �÷��̾� ��Ʈ�ѷ��� �������ش�.
	//PlayerControllerClass = ATitlePlayerController::StaticClass();

	//UNovaGameInstance* gameInstance = Cast<UNovaGameInstance>(GetWorld()->GetGameInstance());

	// ������ �̱��� ��ü�� ������ش�.
	//NovaSocket = NovaSocketClient::GetSingleton();
	//NovaSocket = gameInstance->NovaSocketcli;
	// ������ �ʱ�ȭ�Ѵ�.
	//NovaSocket->Init_Socket();
	// ������ �����Ѵ�. ���� IP�ּҴ� �������ְ�, ��Ʈ��ȣ�� 1004���̴�.
	// ���ῡ �����ϸ� true ���� �����Ѵ�.
	//bIsConnected = gameInstance->bIsConnected;

	// Ÿ��Ʋ �ν��Ͻ��� ���� ������ �������ְ� ����Ʈ�� ����ش�.
	TitleWidgetInstance = CreateWidget<UTitleUserWidget>(GetWorld(), TitleWidgetClass->GetClass());
	TitleWidgetInstance->AddToViewport();

	// �÷��̾� ��Ʈ�ѷ��� ������ش�. 
	APlayerController* PController = GetWorld()->GetFirstPlayerController();
	PController->bShowMouseCursor = true; // ���콺 ǥ��
	PController->bEnableClickEvents = true; // Ŭ���̺�Ʈ ����
	PController->bEnableMouseOverEvents = true; // ���콺 ���� �̺�Ʈ ����

	//FInputModeUIOnly mode;

	if (UNovaGameInstance::bIsConnected == true)
	{
		UNovaGameInstance::bIsConnected = false;
	}
	
}

bool ATitleGameMode::OnSignUp(const FText &Id, const FText &Pw)
{

	if (Id.IsEmpty() || Pw.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("Id, Pw is empty"));
		return false;
	}

	if (!bIsConnected) {
		UE_LOG(LogTemp, Warning, TEXT("Socket Connection Faild!!"));
		return false;
	}

	bool IsSuccess = NovaSocketClient::Sign_up(Id, Pw);

	if (!IsSuccess) {
		UE_LOG(LogTemp, Warning, TEXT("SignUp Faild!!"));
		return false;
	}

	return true;
}

bool ATitleGameMode::OnLogin(const FText &Id, const FText &Pw)
{
	// ���� ���̵�� ��й�ȣ �κ��� ����ִٸ� False �� ����
	if (Id.IsEmpty() || Pw.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("Id, Pw is empty"));
		return false;
	}
	if (UNovaGameInstance::bIsConnected == false) {
		// ������ �̱��� ��ü�� ������ش�.
		UNovaGameInstance::NovaSocketcli = NovaSocketClient::GetSingleton();
		// ������ �ʱ�ȭ�Ѵ�.
		UNovaGameInstance::NovaSocketcli->Init_Socket();
		// ������ �����Ѵ�. ���� IP�ּҴ� �������ְ�, ��Ʈ��ȣ�� 1004���̴�.
		// ���ῡ �����ϸ� true ���� �����Ѵ�.
		UNovaGameInstance::bIsConnected = UNovaGameInstance::NovaSocketcli->Connect_Server(1004);

		
	}



	// ���ϰ� ������ ������ �ִٸ� False �� ����
	if (!UNovaGameInstance::bIsConnected) {
		UE_LOG(LogTemp, Warning, TEXT("Socket Connection Faild!!"));
		return false;
	}

	// ���Ͽ� �α��� �޼ҵ带 ������� ���ϰ��� �޴´�. �α��ο� �����ϸ� true ��ȯ
	bool IsSuccess = NovaSocketClient::Login(Id, Pw);

	// �α��ο� �����ߴٸ� false�� ����
	if (!IsSuccess) {
		UE_LOG(LogTemp, Warning, TEXT("Login Faild!!"));
		return false;
	}
	// �����κ��� ���� �����͸� ó���� ������ ����
	NovaSocketListener::RunSocketListening();

	// FText -> FString���� ��ȯ
	FString ID = *Id.ToString();
	// �����ν��Ͻ��� �����ϱ� ���� �����ͺ����� �������ش�. �׸��� ���� ������ �ν��Ͻ��� ������ ��������ش�.
	UNovaGameInstance* gameInstance = Cast<UNovaGameInstance>(GetWorld()->GetGameInstance());
	gameInstance->userLoginID = std::string(TCHAR_TO_UTF8(*ID)); // �ν��Ͻ��� �α��� ���̵� ����
	UE_LOG(LogTemp, Warning, TEXT("LoginID : %s"), UTF8_TO_TCHAR(gameInstance->userLoginID.c_str()));

	return true;
}
