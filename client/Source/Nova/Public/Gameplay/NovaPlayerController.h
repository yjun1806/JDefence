// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "GameFramework/PlayerController.h"
#include <string>
#include "NovaCharacter.h"
#include "MonsterCharacter.h"
#include "NovaPlayerController.generated.h"

class UPlayHUDWidget;
class UPlayMenuWidget;
class UDiedHUD;
class USystemMessageWidget;
class UGameEndWidget;
/**
 * 
 */
UCLASS()
class NOVA_API ANovaPlayerController : public APlayerController
{
	GENERATED_BODY()

	ANovaPlayerController();
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

	// ������ų �ٸ� ĳ����
	//UPROPERTY(EditAnywhere, Category = "Spawn")
	//TSubclassOf<class A

	UPROPERTY()
	ANovaCharacter* WhoToSpawn;

	UPROPERTY()
	AMonsterCharacter* MonsterSpawn;

	UPROPERTY()
	AMonsterCharacter* Monster_Normal;

	UPROPERTY()
	AMonsterCharacter* Monster_Rare;

	UFUNCTION(BlueprintCallable, Category = "Chat")
	bool WriteMessageToChatWindow(const FString& msg);

	void UpdateNewMessage();

	void UpdateSystemMessage();

	UFUNCTION(BlueprintCallable)
		void Skill1_Using_HUD_Count();

	UFUNCTION(BlueprintCallable)
		void Skill2_Using_HUD_Count();

	UFUNCTION()
		void UpdateTowerHealth();





private:
	UFUNCTION(BlueprintCallable)
	void OnGamePause();

	void SpawnNewPlayerChatacter();

	void SpawnMonster();

	FTimerHandle SendPlayerInfoHandle; // ĳ���͵����͸� ������ �����ϴ� Ÿ�̸� �ڵ鷯�̴�.

	void SendPlayerInfo();

	bool UpdateWorld();

	void StartCounting();

	void UpdateGameTime();




	UFUNCTION()
		void UpdateHPBar();



	UPROPERTY()
		UPlayHUDWidget* PlayHUDClass;

	// �������Ʈ ������ ��Ʈ���� ����
	UPROPERTY()
		class UPlayHUDWidget* PlayHUD;

	UPROPERTY()
		UPlayMenuWidget* MenuWidgetClass;

	UPROPERTY()
	class UPlayMenuWidget* MenuWidget;

	UPROPERTY(VisibleAnywhere, Category = "UI")
		class UUserWidget* CharacterHPBarWidget;

	UPROPERTY(VisibleAnywhere, Category = "UI")
		class UMonsterHPWidget* MonsterHPBarWidget;


	UPROPERTY()
		UDiedHUD* DiedHUDClass;

	// �������Ʈ ������ ��Ʈ���� ����
	UPROPERTY()
		class UDiedHUD* DiedHUD;


	UPROPERTY()
		UGameEndWidget* GameEndClass;

	// �������Ʈ ������ ��Ʈ���� ����
	UPROPERTY()
		class UGameEndWidget* GameEndHUD;

	UPROPERTY()
		UGameEndWidget* GameWinClass;

	// �������Ʈ ������ ��Ʈ���� ����
	UPROPERTY()
		class UGameEndWidget* GameWinHUD;

protected:
	UPROPERTY()
	bool gamestart;
};
