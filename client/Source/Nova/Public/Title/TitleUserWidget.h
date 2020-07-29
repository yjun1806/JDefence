// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "NovaSocketClient.h"
#include "TitleUserWidget.generated.h"

/**
 *	블루프린트 위젯 (UI_Title)의 부모클래스이다.
 */



UCLASS()
class NOVA_API UTitleUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnLoginClicked();

	/*UFUNCTION(BlueprintCallable, Category = "Add")
	int32 OnMembershipAddClicked();*/

private:
	UPROPERTY()
	class UButton* LoginButton;

	UPROPERTY()
	class UButton* MembershipAddButton;

	UPROPERTY()
	class UButton* Add_chk;

	UPROPERTY()
	class UEditableTextBox* LoginText;

	UPROPERTY()
	class UEditableTextBox* Add_ID;

	UPROPERTY()
	class UEditableTextBox* Add_Pw;

	UPROPERTY()
	class UEditableTextBox* Add_Pw_chk;

	UPROPERTY()
	class UEditableTextBox* NoticeText;

	UPROPERTY()
	class UBorder* TitleBorder;


private:
	//NovaSocketClient* NovaSocket;



};
