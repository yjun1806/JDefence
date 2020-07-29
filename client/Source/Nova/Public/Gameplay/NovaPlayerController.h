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

	// 스폰시킬 다른 캐릭터
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

	FTimerHandle SendPlayerInfoHandle; // 캐릭터데이터를 서버에 전송하는 타이머 핸들러이다.

	void SendPlayerInfo();

	bool UpdateWorld();

	void StartCounting();

	void UpdateGameTime();




	UFUNCTION()
		void UpdateHPBar();



	UPROPERTY()
		UPlayHUDWidget* PlayHUDClass;

	// 블루프린트 위젯을 컨트롤할 변수
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

	// 블루프린트 위젯을 컨트롤할 변수
	UPROPERTY()
		class UDiedHUD* DiedHUD;


	UPROPERTY()
		UGameEndWidget* GameEndClass;

	// 블루프린트 위젯을 컨트롤할 변수
	UPROPERTY()
		class UGameEndWidget* GameEndHUD;

	UPROPERTY()
		UGameEndWidget* GameWinClass;

	// 블루프린트 위젯을 컨트롤할 변수
	UPROPERTY()
		class UGameEndWidget* GameWinHUD;

protected:
	UPROPERTY()
	bool gamestart;
};
