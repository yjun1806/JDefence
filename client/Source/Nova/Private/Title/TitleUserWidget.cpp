// Fill out your copyright notice in the Description page of Project Settings.

#include "TitleUserWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"






void UTitleUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 로그인 버튼을 눌렀을때
	LoginButton = Cast<UButton>(GetWidgetFromName(TEXT("TitleLogin")));
	if (nullptr != LoginButton)
	{
		//LoginButton->OnClicked.AddDynamic(this, &UTitleUserWidget::OnLoginClicked);
		//LoginButton->OnClicked.AddDynamic(this, &UNovaClient::connect_server);
	}

	MembershipAddButton = Cast<UButton>(GetWidgetFromName(TEXT("TitleMembershipAdd")));
	ABCHECK(nullptr != MembershipAddButton);

	// 회원가입 완료 버튼을 눌렀을때
	Add_chk = Cast<UButton>(GetWidgetFromName(TEXT("MembershipAddOkBtn")));
	if (nullptr != Add_chk)
	{
		//Add_chk->OnClicked.AddDynamic(this, &UTitleUserWidget::OnMembershipAddClicked);
	}

	LoginText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Title_Login_TextBox")));
	ABCHECK(nullptr != LoginText);

	Add_ID = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Title_Add_ID")));
	ABCHECK(nullptr != Add_ID);

	Add_Pw = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Title_Add_Pw")));
	ABCHECK(nullptr != Add_Pw);

	Add_Pw_chk = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Title_Add_Pw_ck")));
	ABCHECK(nullptr != Add_Pw_chk);

	NoticeText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Notice_Text")));
	ABCHECK(nullptr != Add_Pw_chk);

	TitleBorder = Cast<UBorder>(GetWidgetFromName(TEXT("Title_Border")));

	
	//
	/*NovaSocket = NovaSocketClient::GetSingleton();
	NovaSocket->Init_Socket();
	NovaSocket->Connect_Server(1004);*/

}

void UTitleUserWidget::OnLoginClicked()
{
	//UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lobby"));

	//nc->connect_server();

	/*FIPv4Address ip(127, 0, 0, 1);
	TSharedRef <FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(1004);

	bool connected = Socket->Connect(*addr);

	FString serialized = LoginText->GetText().ToString();
	TCHAR* serializedChar = serialized.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	int32 sent = 0;

	bool successful = Socket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);*/
}

//int32 UTitleUserWidget::OnMembershipAddClicked()
//{
//	// 회원가입 완료 버튼을 눌렀을때 동작
//	
//	if (Add_ID->GetText().IsEmpty() || Add_Pw->GetText().IsEmpty() || Add_Pw_chk->GetText().IsEmpty())
//	{
//		// 칸이 비어있는 경우
//		UE_LOG(LogTemp, Warning, TEXT("Return 1"));
//		return 1;
//	}
//
//		
//	if (!Add_Pw->GetText().EqualTo(Add_Pw_chk->GetText()))
//	{
//		// 비밀번호가 일치하지 않는 경우
//		UE_LOG(LogTemp, Warning, TEXT("Return 2"));
//		return 2;
//	}
//	
//	
//	bool SignUp = NovaSocket->Sign_up(Add_ID->GetText().ToString(), Add_Pw->GetText().ToString());
//
//
//	if (SignUp == true) {
//		return 3; // 아이디가 없는 경우 회원 가입 성공
//		UE_LOG(LogTemp, Warning, TEXT("Return 3"));
//	}
//	
//
//	// 회원가입 실패 , 아이디가 있는 경우
//	UE_LOG(LogTemp, Warning, TEXT("Return 0"));
//	return 0;
//}





