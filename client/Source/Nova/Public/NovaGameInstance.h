// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Engine/GameInstance.h"
#include <string>
#include "NovaSocketClient.h"
#include "SocketSubsystem.h"
#include "PlayerInfo.h"
#include "RoomInfoCls.h"
#include "RoomUserInfoCls.h"
#include "PlayerDataClass.h"
#include "MonsterDataClass.h"
#include "MonsterCharacter.h"
#include "flatbuffers/flatbuffers.h"
#include "Flatbuf/FlatPacket_generated.h"
#include "NovaGameInstance.generated.h"




/**
 * 
 */
UCLASS()
class NOVA_API UNovaGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UNovaGameInstance();
	
	virtual void Init() override;

	virtual void Shutdown() override;

public:
	UFUNCTION(BlueprintCallable)
		bool IsGameStart();

	static int32 IDcode; // ���� �α����� Ŭ���̾�Ʈ�� ������ ���̵��ڵ�, ������

	static std::string sessionId;
	static std::string userLoginID;
	static std::string ReceiveMsgUserID;
	static std::string ReceiveChatMessage;
	static std::string ReceiveSystemMessage;
	static bool IsNeedUpdateMessage;
	static bool LobbyUserListUpdate;
	static bool LobbyRoomListUpdate;
	static bool GamePlayerUpdate;
	static bool GameMonsterUpdate;
	static bool PlayerListChanged;
	static bool IsNowGamePlaying;
	static bool UpdateSystemMessage;
	static bool GameCounting; // ���� ������ �ϱ� ���� ī�����ϴ� �κ�
	static bool RoomUserListUpdate; // ��ȿ��� ������ ���� ������Ʈ �κ�
	static bool RoomMakeWaiting; // �游��� ��ư�� ������, �������� ���ȣ�� �����ؼ� �����ٶ����� ��ٸ��� �ϴ� ����ġ
	static bool UpdateTowerHP;
	static bool UpdateGameTime;
	static bool UpdateTopRanking;

	static bool IsPasswordCorrect;
	static bool ReceivedPasswordCheckData;
	static int32 ItemBuySucessful;


	static int32 NowPlayStateCode; // ���� ���� ���������� ������ ����
	static int32 MyRoomNumber; // ���� ���� ���̰ų� ���� ������ ���� ��ȣ�� ������ ����
	static int32 CountingNumber; // ���� ī��Ʈ ���� ����.
	static int32 SystemMessageCode; // ���� ī��Ʈ ���� ����.
	static int32 TowerHP;
	static int32 GameTime;


	// ���� ���� ���ӻ��¿� �ִ� �������� ������ �����ϴ� �迭
	static TArray<class PlayerInfo*> PlayerList;

	// ž��ŷ�� �����ϴ� �迭
	static TArray<class PlayerInfo*> TopRanker;

	// ������������ �÷��̾� ����Ʈ ������ �����ϴ� �迭
	//static TArray<class ANovaCharacter*> PlayingGameUserList;
	static TMap<int32, class ANovaCharacter*> PlayingGameUserList;
	// ���� �����Ǿ� �ִ� ������ ������ �����ϴ� �迭
	//static TArray<class AMonsterCharacter*> MonsterList;
	static TMap<int32, class AMonsterCharacter*> MonsterList;


	static int32 SpawnNeedIndex;
	static PlayerDataClass TempPlayerData;
	static MonsterDataClass TempMonsterData;
	static ANovaCharacter* MyCharacterData;

	static object::PlayerData* TempPD;

	static RoomInfoCls* MyRoomInfo; // ���� �������� �������� �����ϴ� ����(���� ������ϼ��� �ְ�, ���� ������ ���ϼ��� �ִ�.)
	static TArray<class RoomInfoCls*> TempRoomInfo; // �ٸ� Ŭ���̾�Ʈ�� �������� �ӽ� �����ϴ� ����
	static int32 GetPlayerListNumber();

public:
	static NovaSocketClient* NovaSocketcli;
	static bool bIsConnected;

	UFUNCTION(BlueprintCallable)
	static FString GetUserID();

	static FSocket* GetNovaSocket();

	

	UFUNCTION()
		void StartGarbegeSwaper();

	UFUNCTION()
		void CheckMonster();

	UFUNCTION()
		void ClearCheackMonsterTimer();

	UPROPERTY()
		FTimerHandle CheckMonsterHandler;
	

public:
	static int32 KilledMonsterValue; // ���� ���� ��
	static int32 KilledMonsterValue_Rare; // ���� ���� ��
	static int32 KilledMonsterValue_Boss; // ���� ���� ��

	static int32 GameEndLeftTime; // ���ӳ����� ���� ���� �ð�(���ӿ����ΰ�� ���� �ð� ����)

	static int32 GameEndLeftTowerHP; // ���ӳ����� ���� Ÿ�� ü��(���ӽ¸��� ��� ���� Ÿ��ü������)

	static int32 DiedValue; // ���� Ƚ��

	static int32 TotalReceivedDamage; // �� ���� �����

	static int32 TotalGaindDamage; // ���Ϳ��� ���� �����
	
	static int32 CurrentPoint; // ���Ϳ��� ���� �����
	   
	static int32 Rank1_Point;

	static int32 Rank2_Point;

	static int32 Rank3_Point;

	static int32 Rank4_Point;

	static FString Rank1_ID;

	static FString Rank2_ID;

	static FString Rank3_ID;

	static FString Rank4_ID;

	UFUNCTION(BlueprintCallable)
		int32 GetRankPoint(int32 who);

	UFUNCTION(BlueprintCallable)
		FString GetRankID(int32 who);

	UFUNCTION(BlueprintCallable)
		int32 GetKilledMonsterValue();

	UFUNCTION(BlueprintCallable)
		int32 GetKilledMonsterValue_Rare();

	UFUNCTION(BlueprintCallable)
		int32 GetKilledMonsterValue_Boss();

	UFUNCTION(BlueprintCallable)
		int32 GetGameEndLeftTime();

	UFUNCTION(BlueprintCallable)
		int32 GetGameEndLeftTowerHP();

	UFUNCTION(BlueprintCallable)
		int32 GetGameDiedValue();

	UFUNCTION(BlueprintCallable)
		int32 GetTotalReceivedDamage();

	UFUNCTION(BlueprintCallable)
		int32 GetTotalGainDamage();

	static int32 MaxPoint;

	UFUNCTION(BlueprintCallable)
		int32 GetMaxPoint();

	UFUNCTION(BlueprintCallable)
		int32 GetCurrentPoint();

	UFUNCTION(BlueprintCallable)
		int32 GetCurrentGold();
	
	static int32 MyCurrentGold;
	static int32 CurrentPotion100;
	static int32 CurrentPotion50;

	UFUNCTION(BlueprintCallable)
		int32 Get100Potion();

	UFUNCTION(BlueprintCallable)
		int32 Get50Potion();



};





