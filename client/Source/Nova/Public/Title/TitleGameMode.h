// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "GameFramework/GameModeBase.h"
#include "NovaSocketClient.h"
#include "TitleGameMode.generated.h"

class UTitleUserWidget;

/**
 *  타이틀 레벨의 게임모드 설정 부분이다.
	타이틀에서 회원가입, 로그인 및 타이틀을 보여주는 블루프린트 위젯을 표시할 것이다.
	게임모드에서 플레이어 컨트롤러를 연결해준다.
 */
UCLASS()
class NOVA_API ATitleGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	ATitleGameMode();

	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "SignUp")
	bool OnSignUp(const FText &Id, const FText &Pw);

	UFUNCTION(BlueprintCallable, Category = "Login")
	bool OnLogin(const FText &Id, const FText &Pw);

private:
	NovaSocketClient* NovaSocket;
	bool bIsConnected;

protected:
	// 블루프린트 위젯을 연결해줄 속성
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UTitleUserWidget> TitleWidgetClass;*/
	UPROPERTY()
	UTitleUserWidget* TitleWidgetClass;


	// 블루프린트 위젯을 컨트롤할 변수
	UPROPERTY()
	class UTitleUserWidget* TitleWidgetInstance;


};
