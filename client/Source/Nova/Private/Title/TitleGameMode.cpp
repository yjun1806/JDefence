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
	// 블루프린트로 제작된 블루프린트 위젯을 연결해준다.
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
	// 게임모드에서 사용할 플레이어 컨트롤러를 지정해준다.
	//PlayerControllerClass = ATitlePlayerController::StaticClass();

	//UNovaGameInstance* gameInstance = Cast<UNovaGameInstance>(GetWorld()->GetGameInstance());

	// 소켓을 싱글톤 객체로 만들어준다.
	//NovaSocket = NovaSocketClient::GetSingleton();
	//NovaSocket = gameInstance->NovaSocketcli;
	// 소켓을 초기화한다.
	//NovaSocket->Init_Socket();
	// 소켓을 연결한다. 현재 IP주소는 정해져있고, 포트번호는 1004번이다.
	// 연결에 성공하면 true 값을 리턴한다.
	//bIsConnected = gameInstance->bIsConnected;

	// 타이틀 인스턴스를 만들어서 위젯을 생성해주고 뷰포트에 띄워준다.
	TitleWidgetInstance = CreateWidget<UTitleUserWidget>(GetWorld(), TitleWidgetClass->GetClass());
	TitleWidgetInstance->AddToViewport();

	// 플레이어 컨트롤러를 만들어준다. 
	APlayerController* PController = GetWorld()->GetFirstPlayerController();
	PController->bShowMouseCursor = true; // 마우스 표시
	PController->bEnableClickEvents = true; // 클릭이벤트 가능
	PController->bEnableMouseOverEvents = true; // 마우스 오버 이벤트 가능

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
	// 만약 아이디와 비밀번호 부분이 비어있다면 False 값 리턴
	if (Id.IsEmpty() || Pw.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("Id, Pw is empty"));
		return false;
	}
	if (UNovaGameInstance::bIsConnected == false) {
		// 소켓을 싱글톤 객체로 만들어준다.
		UNovaGameInstance::NovaSocketcli = NovaSocketClient::GetSingleton();
		// 소켓을 초기화한다.
		UNovaGameInstance::NovaSocketcli->Init_Socket();
		// 소켓을 연결한다. 현재 IP주소는 정해져있고, 포트번호는 1004번이다.
		// 연결에 성공하면 true 값을 리턴한다.
		UNovaGameInstance::bIsConnected = UNovaGameInstance::NovaSocketcli->Connect_Server(1004);

		
	}



	// 소켓과 연결이 끊어져 있다면 False 값 리턴
	if (!UNovaGameInstance::bIsConnected) {
		UE_LOG(LogTemp, Warning, TEXT("Socket Connection Faild!!"));
		return false;
	}

	// 소켓에 로그인 메소드를 실행시켜 리턴값을 받는다. 로그인에 성공하면 true 반환
	bool IsSuccess = NovaSocketClient::Login(Id, Pw);

	// 로그인에 실패했다면 false값 리턴
	if (!IsSuccess) {
		UE_LOG(LogTemp, Warning, TEXT("Login Faild!!"));
		return false;
	}
	// 서버로부터 받은 데이터를 처리할 스레드 실행
	NovaSocketListener::RunSocketListening();

	// FText -> FString으로 변환
	FString ID = *Id.ToString();
	// 게임인스턴스에 접근하기 위해 포인터변수를 선언해준다. 그리고 현재 게임의 인스턴스를 가져와 연결시켜준다.
	UNovaGameInstance* gameInstance = Cast<UNovaGameInstance>(GetWorld()->GetGameInstance());
	gameInstance->userLoginID = std::string(TCHAR_TO_UTF8(*ID)); // 인스턴스에 로그인 아이디 저장
	UE_LOG(LogTemp, Warning, TEXT("LoginID : %s"), UTF8_TO_TCHAR(gameInstance->userLoginID.c_str()));

	return true;
}
