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

	static int32 IDcode; // 현재 로그인한 클라이언트의 고유한 아이디코드, 정수형

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
	static bool GameCounting; // 게임 시작을 하기 위해 카운팅하는 부분
	static bool RoomUserListUpdate; // 방안에서 참여자 정보 업데이트 부분
	static bool RoomMakeWaiting; // 방만들기 버튼을 누르고, 서버에서 방번호를 생성해서 돌려줄때까지 기다리게 하는 스위치
	static bool UpdateTowerHP;
	static bool UpdateGameTime;
	static bool UpdateTopRanking;

	static bool IsPasswordCorrect;
	static bool ReceivedPasswordCheckData;
	static int32 ItemBuySucessful;


	static int32 NowPlayStateCode; // 현재 나의 상태정보를 저장할 변수
	static int32 MyRoomNumber; // 내가 만든 방이거나 내가 참여한 방의 번호를 저장할 변수
	static int32 CountingNumber; // 현재 카운트 중인 숫자.
	static int32 SystemMessageCode; // 현재 카운트 중인 숫자.
	static int32 TowerHP;
	static int32 GameTime;


	// 현재 같은 게임상태에 있는 유저들의 정보를 저장하는 배열
	static TArray<class PlayerInfo*> PlayerList;

	// 탑랭킹을 저장하는 배열
	static TArray<class PlayerInfo*> TopRanker;

	// 게임진행중인 플레이어 리스트 정보를 저장하는 배열
	//static TArray<class ANovaCharacter*> PlayingGameUserList;
	static TMap<int32, class ANovaCharacter*> PlayingGameUserList;
	// 현재 스폰되어 있는 몬스터의 정보를 저장하는 배열
	//static TArray<class AMonsterCharacter*> MonsterList;
	static TMap<int32, class AMonsterCharacter*> MonsterList;


	static int32 SpawnNeedIndex;
	static PlayerDataClass TempPlayerData;
	static MonsterDataClass TempMonsterData;
	static ANovaCharacter* MyCharacterData;

	static object::PlayerData* TempPD;

	static RoomInfoCls* MyRoomInfo; // 현재 참여중인 방정보를 저장하는 변수(내가 만든방일수도 있고, 내가 참여한 방일수도 있다.)
	static TArray<class RoomInfoCls*> TempRoomInfo; // 다른 클라이언트의 방정보를 임시 저장하는 변수
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
	static int32 KilledMonsterValue; // 몬스터 죽인 수
	static int32 KilledMonsterValue_Rare; // 몬스터 죽인 수
	static int32 KilledMonsterValue_Boss; // 몬스터 죽인 수

	static int32 GameEndLeftTime; // 게임끝날때 남은 게임 시간(게임오버인경우 남는 시간 존재)

	static int32 GameEndLeftTowerHP; // 게임끝날때 남은 타워 체력(게임승리인 경우 남은 타워체력존재)

	static int32 DiedValue; // 죽은 횟수

	static int32 TotalReceivedDamage; // 총 받은 대미지

	static int32 TotalGaindDamage; // 몬스터에게 입힌 대미지
	
	static int32 CurrentPoint; // 몬스터에게 입힌 대미지
	   
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





