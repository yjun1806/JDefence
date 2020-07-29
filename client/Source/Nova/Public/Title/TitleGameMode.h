// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "GameFramework/GameModeBase.h"
#include "NovaSocketClient.h"
#include "TitleGameMode.generated.h"

class UTitleUserWidget;

/**
 *  Ÿ��Ʋ ������ ���Ӹ�� ���� �κ��̴�.
	Ÿ��Ʋ���� ȸ������, �α��� �� Ÿ��Ʋ�� �����ִ� �������Ʈ ������ ǥ���� ���̴�.
	���Ӹ�忡�� �÷��̾� ��Ʈ�ѷ��� �������ش�.
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
	// �������Ʈ ������ �������� �Ӽ�
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UTitleUserWidget> TitleWidgetClass;*/
	UPROPERTY()
	UTitleUserWidget* TitleWidgetClass;


	// �������Ʈ ������ ��Ʈ���� ����
	UPROPERTY()
	class UTitleUserWidget* TitleWidgetInstance;


};
