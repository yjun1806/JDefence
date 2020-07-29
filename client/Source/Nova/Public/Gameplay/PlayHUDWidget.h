// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "PlayHUDWidget.generated.h"

/**
 * ���� ����� ǥ�õǴ� ü�¹�, ����ġ��, ����Ʈ ����� ǥ������ HUD ������ ��Ʈ���ϴ� Ŭ�����̴�.
 */

class ANovaCharacter;
UCLASS()
class NOVA_API UPlayHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	// NativeConstruct�� ����Ŭ������ �ε��ɶ� ȣ��ȴ�.
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	class UTextBlock* playerNameHUD;

	UPROPERTY()
	class UTextBlock* WaitingText;

	UPROPERTY()
	class UTextBlock* WaitingCount;

	UPROPERTY()
		class UTextBlock* CurrentMonNum;

	UPROPERTY()
	class UBackgroundBlur* WaitingPanel;

public:
	UPROPERTY()
		TArray<class UTextBlock*> playername;

	UPROPERTY()
		TArray<class UBorder*> Player_border;

	/*UPROPERTY()
		TArray<class UScaleBox*> PlayerOwnerImg;*/

	UPROPERTY()
		TArray<class UProgressBar*> PlayerHPbar;



	
public:
	UFUNCTION()
		void PlayStartCounting();

	UFUNCTION()
		void RemoveWaitingPanel();

	UFUNCTION()
		void WaitingPanelSetBlur();

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateCurrentPoint();

	UFUNCTION(BlueprintImplementableEvent)
		void SetWaitingText();

	UFUNCTION(BlueprintImplementableEvent, Category = "ReadMesg")
		void UpdateChatmsg(const FText& senderid, const FText& msg);

	UFUNCTION(BlueprintImplementableEvent, Category = "ReadMesg")
		void UpdateSysmsg(const FText& msg, const int32 code);

	UFUNCTION(BlueprintImplementableEvent)
		void Init();

	UFUNCTION()
	void SetPlayersHUD();

	UFUNCTION()
		void UpdateHPbar(const ANovaCharacter* chr);

	UFUNCTION()
		float CalculateRatio(float MaxHp, float CurrentHP);


	UFUNCTION(BlueprintImplementableEvent)
		void StartSkill1_Count();

	UFUNCTION(BlueprintImplementableEvent)
		void StartSkill2_Count();

	UFUNCTION(BlueprintImplementableEvent)
		void SetTowerHP(int32 Health);

	UFUNCTION(BlueprintImplementableEvent)
		void SetGameTime(int32 Time);

	UFUNCTION(BlueprintCallable)
		void Using_100Potion();

	UFUNCTION(BlueprintCallable)
		void Using_50Potion();

	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePotion();
};
