// Fill out your copyright notice in the Description page of Project Settings.

#include "NovaGameInstance.h"
#include "NovaSocketClient.h"
#include "NovaSocketListener.h"
#include "TimerManager.h"




bool UNovaGameInstance::IsGameStart()
{
	return UNovaGameInstance::GameCounting;
}

//TODO : Ŭ���̾�Ʈ�� ó�� ������ ������ �����ϴ�â ����� �׸��� ������ ���ӽõ��� �ϰ� ����� ��쿡�� �α��� �� ȸ�������� �� �� �ֵ��� �����(��ƺ�� ��������âó��)

std::string UNovaGameInstance::sessionId = "";
std::string UNovaGameInstance::userLoginID = "";

std::string UNovaGameInstance::ReceiveMsgUserID = "";
std::string UNovaGameInstance::ReceiveChatMessage = "";

std::string UNovaGameInstance::ReceiveSystemMessage = "";

bool UNovaGameInstance::IsNeedUpdateMessage = false;

bool UNovaGameInstance::LobbyUserListUpdate = false;

bool UNovaGameInstance::LobbyRoomListUpdate = false;

bool UNovaGameInstance::GamePlayerUpdate = false;

bool UNovaGameInstance::GameMonsterUpdate = false;

bool UNovaGameInstance::PlayerListChanged = false;

bool UNovaGameInstance::IsNowGamePlaying = false;

bool UNovaGameInstance::UpdateSystemMessage = false;

bool UNovaGameInstance::GameCounting = false;

bool UNovaGameInstance::RoomUserListUpdate = false;

bool UNovaGameInstance::RoomMakeWaiting = false;

bool UNovaGameInstance::UpdateTowerHP = false;

bool UNovaGameInstance::UpdateGameTime = false;

bool UNovaGameInstance::UpdateTopRanking = false;

bool UNovaGameInstance::IsPasswordCorrect = false;
bool UNovaGameInstance::ReceivedPasswordCheckData = false;


int32 UNovaGameInstance::ItemBuySucessful = 0;

int32 UNovaGameInstance::NowPlayStateCode = -1;

int32 UNovaGameInstance::MyRoomNumber = -1;

int32 UNovaGameInstance::CountingNumber = -1;

int32 UNovaGameInstance::SystemMessageCode = 0;

int32 UNovaGameInstance::IDcode = 0; // 0�̸� �̷α��λ���, �α����� ���̵�� 1�̻�, �������� ���ؼ� ������.

int32 UNovaGameInstance::TowerHP = 3000;

int32 UNovaGameInstance::GameTime = 180;

TArray<class PlayerInfo*> UNovaGameInstance::PlayerList;

TArray<class PlayerInfo*> UNovaGameInstance::TopRanker;

//TArray<class ANovaCharacter*> UNovaGameInstance::PlayingGameUserList;
TMap<int32, class ANovaCharacter*> UNovaGameInstance::PlayingGameUserList;

//TArray<class AMonsterCharacter*> UNovaGameInstance::MonsterList;

TMap<int32, class AMonsterCharacter*> UNovaGameInstance::MonsterList;

int32 UNovaGameInstance::SpawnNeedIndex = -1;

PlayerDataClass UNovaGameInstance::TempPlayerData;

MonsterDataClass UNovaGameInstance::TempMonsterData;

ANovaCharacter* UNovaGameInstance::MyCharacterData;

object::PlayerData* UNovaGameInstance::TempPD;

RoomInfoCls* UNovaGameInstance::MyRoomInfo = new RoomInfoCls();


TArray<class RoomInfoCls*> UNovaGameInstance::TempRoomInfo;

int32 UNovaGameInstance::KilledMonsterValue = 0;
int32 UNovaGameInstance::KilledMonsterValue_Rare = 0;
int32 UNovaGameInstance::KilledMonsterValue_Boss = 0;
int32 UNovaGameInstance::GameEndLeftTime = 180;
int32 UNovaGameInstance::GameEndLeftTowerHP = 3000;
int32 UNovaGameInstance::DiedValue = 0;
int32 UNovaGameInstance::TotalReceivedDamage = 0;
int32 UNovaGameInstance::TotalGaindDamage = 0;
int32 UNovaGameInstance::CurrentPoint = 0;
int32 UNovaGameInstance::Rank1_Point = 0;
int32 UNovaGameInstance::Rank2_Point = 0;
int32 UNovaGameInstance::Rank3_Point = 0;
int32 UNovaGameInstance::Rank4_Point = 0;
FString UNovaGameInstance::Rank1_ID = "";
FString UNovaGameInstance::Rank2_ID = "";
FString UNovaGameInstance::Rank3_ID = "";
FString UNovaGameInstance::Rank4_ID = "";
int32 UNovaGameInstance::MaxPoint = 0;



int32 UNovaGameInstance::GetPlayerListNumber()
{
	return PlayerList.Num();
}

//TArray<class AnotherPlayerInfo*> UNovaGameInstance::PlayerList;

NovaSocketClient* UNovaGameInstance::NovaSocketcli;

bool UNovaGameInstance::bIsConnected;

FString UNovaGameInstance::GetUserID()
{
	return UNovaGameInstance::userLoginID.c_str();
}

FSocket* UNovaGameInstance::GetNovaSocket()
{
	return NovaSocketcli->NovaSocket;
}

void UNovaGameInstance::StartGarbegeSwaper()
{
	ABLOG(Error, TEXT("Monster Garbege Swaper Start!!!"));
	GetWorld()->GetTimerManager().SetTimer(CheckMonsterHandler, this, &UNovaGameInstance::CheckMonster, 2.0f, true);
}

void UNovaGameInstance::CheckMonster()
{
	ABLOG(Error, TEXT("Current Monster num: %d"), UNovaGameInstance::MonsterList.Num());
}

void UNovaGameInstance::ClearCheackMonsterTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(CheckMonsterHandler);

}

int32 UNovaGameInstance::GetRankPoint(int32 who)
{
	int32 result = 0;
	switch (who) {
	case 1:
		result = UNovaGameInstance::Rank1_Point;
		break;
	case 2:
		result = UNovaGameInstance::Rank2_Point;
		break;
	case 3:
		result = UNovaGameInstance::Rank3_Point;
		break;
	case 4:
		result = UNovaGameInstance::Rank4_Point;
		break;
	}
	return result;
}

FString UNovaGameInstance::GetRankID(int32 who)
{
	FString result = "";
	switch (who) {
	case 1:
		result = UNovaGameInstance::Rank1_ID;
		break;
	case 2:
		result = UNovaGameInstance::Rank2_ID;
		break;
	case 3:
		result = UNovaGameInstance::Rank3_ID;
		break;
	case 4:
		result = UNovaGameInstance::Rank4_ID;
		break;
	}
	return result;
}

int32 UNovaGameInstance::GetKilledMonsterValue()
{
	return UNovaGameInstance::KilledMonsterValue;
}

int32 UNovaGameInstance::GetKilledMonsterValue_Rare()
{
	return UNovaGameInstance::KilledMonsterValue_Rare;
}

int32 UNovaGameInstance::GetKilledMonsterValue_Boss()
{
	return UNovaGameInstance::KilledMonsterValue_Boss;
}

int32 UNovaGameInstance::GetGameEndLeftTime()
{
	return UNovaGameInstance::GameEndLeftTime;
}

int32 UNovaGameInstance::GetGameEndLeftTowerHP()
{
	return UNovaGameInstance::GameEndLeftTowerHP;
}

int32 UNovaGameInstance::GetGameDiedValue()
{
	return UNovaGameInstance::DiedValue;
}

int32 UNovaGameInstance::GetTotalReceivedDamage()
{
	return UNovaGameInstance::TotalReceivedDamage;
}

int32 UNovaGameInstance::GetTotalGainDamage()
{
	return UNovaGameInstance::TotalGaindDamage;
}

int32 UNovaGameInstance::GetMaxPoint()
{
	return UNovaGameInstance::MaxPoint;
}

int32 UNovaGameInstance::GetCurrentPoint()
{
	return UNovaGameInstance::CurrentPoint;
}

int32 UNovaGameInstance::GetCurrentGold()
{
	return UNovaGameInstance::MyCurrentGold;
}

int32 UNovaGameInstance::MyCurrentGold = 0;

int32 UNovaGameInstance::CurrentPotion100 = 0;

int32 UNovaGameInstance::CurrentPotion50 = 0;

int32 UNovaGameInstance::Get100Potion()
{
	return UNovaGameInstance::CurrentPotion100;
}

int32 UNovaGameInstance::Get50Potion()
{
	return UNovaGameInstance::CurrentPotion50;
}

UNovaGameInstance::UNovaGameInstance()
{
	//KilledMonsterValue = 0;
	//GameEndLeftTime = 180; // �������� �ð��� 180���̹Ƿ�
	//GameEndLeftTowerHP = 3000; // Ÿ���� �ʱ� ü���� 3000
	//DiedValue = 0;
	//TotalReceivedDamage = 0;
	//TotalGaindDamage = 0;
}

// �ν��Ͻ� �ʱ�ȭ �κ�
void UNovaGameInstance::Init()
{
	GLog->Log("UNovaGameInstance::Init()");
	//// ������ �̱��� ��ü�� ������ش�.
	//UNovaGameInstance::NovaSocketcli = NovaSocketClient::GetSingleton();
	//// ������ �ʱ�ȭ�Ѵ�.
	//UNovaGameInstance::NovaSocketcli->Init_Socket();
	//// ������ �����Ѵ�. ���� IP�ּҴ� �������ְ�, ��Ʈ��ȣ�� 1004���̴�.
	//// ���ῡ �����ϸ� true ���� �����Ѵ�.
	//UNovaGameInstance::bIsConnected = NovaSocketcli->Connect_Server(1004);

	//// �����κ��� ���� �����͸� ó���� ������ ����
	//NovaSocketListener::RunSocketListening();

	UNovaGameInstance::PlayerList.Empty();
	UNovaGameInstance::PlayingGameUserList.Empty();

}

// ���� ����� �߻��Ǵ� �̺�Ʈ, ���� ������ �����ش�.
void UNovaGameInstance::Shutdown()
{
	GLog->Log("UNovaGameInstance::Shutdown()");
	if (UNovaGameInstance::bIsConnected == true) {
		
		if (UNovaGameInstance::NowPlayStateCode >= 100) {
			//for (int i = 0; i < UNovaGameInstance::PlayingGameUserList.Num(); i++)
			//{
			//	//UE_LOG(LogTemp, Warning, TEXT("BackToLobby! / PlayingGameUserNum %d"), UNovaGameInstance::PlayingGameUserList.Num());
			//	UNovaGameInstance::PlayingGameUserList[i]->Destroy();
			//}

			/*for (int i = 0; i < UNovaGameInstance::MonsterList.Num(); i++)
			{
				UNovaGameInstance::MonsterList[i]->Destroy();
			}*/
			UNovaGameInstance::MonsterList.Empty();
			UNovaGameInstance::PlayingGameUserList.Empty();
			UNovaGameInstance::IsNowGamePlaying = false;
			UNovaGameInstance::NovaSocketcli->SendLeaveRoomInfo(UNovaGameInstance::NowPlayStateCode);
		
		
		}
		if (UNovaGameInstance::NowPlayStateCode != -1) {
			UNovaGameInstance::NowPlayStateCode = 3; // �켱 �����ڵ带 �ٲ��ش�. // �α׾ƿ� 3
		}
		UNovaGameInstance::MyRoomNumber = -1;
		UNovaGameInstance::NovaSocketcli->SendGameState(UNovaGameInstance::NowPlayStateCode);

		UE_LOG(LogTemp, Warning, TEXT("GAME EXIT // bIsConnected : %d"), bIsConnected);
	}
	NovaSocketListener::Shutdown();
}




//AnotherPlayerInfo::AnotherPlayerInfo(string Userid, int32 gamecode)
//{
//	this->Userid = Userid;
//	this->NowGameStateCode = gamecode;
//}
