// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Networking.h"
#include <string>
#include "Runtime/Sockets/Public/IPAddress.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"


//#include "SocketSubsystem.h"

using namespace std;

/**
 * 
 */
class NOVA_API NovaSocketClient
{
public:
	NovaSocketClient();
	~NovaSocketClient();


public:
	static FSocket* NovaSocket;

	// �̱��� ��ü ��������
	static NovaSocketClient* GetSingleton()
	{
		static NovaSocketClient ins;
		return &ins;
	}


	static bool Init_Socket();
	static bool Connect_Server(int Port);
	static bool DisConnect_Server();
	static bool Sign_up(const FText &Id, const FText &Pw);
	static bool Login(const FText &Id, const FText &Pw);

	static bool SendGameState(int32 code);
	//
	//static bool SendObjectPacket(ObjectPacket &sendpk);

	static bool SendMessageToServer(const FString &text, int32 gamestatecode, string UserID);

	//static bool ReadDelimitedFrom(google::protobuf::io::CodedInputStream * input, google::protobuf::MessageLite * message);
	
	// �ڽ��� ĳ���� ���¸� ������ �޼ҵ�
	//static bool SendCharaterState(PlayerData* data);

	//static bool SendMonsterData(MonsterData* data);

	// ���� ������ �������� ������ �޼ҵ�
	static bool SendRoomInfo();

	// ���� �غ����� �ƴ��� ������ �޼ҵ�
	static bool SendGameReadyInfo(int32 roomnumber, bool isready);

	// ���� ������ ��� ������ �޼ҵ�
	static bool SendLeaveRoomInfo(int32 roomnumber);

	// �濡�� ���� ���� ��ư�� ������������ ����Ǵ� ������ ���õ� �޼ҵ�
	static bool SendGameStartInfo(int32 roomnumber, bool isstart);

	static bool SendGameLoadingComplete(int32 roomnumber, bool isloadingcomplete);

	static bool SendAttackInfo(int32 Attacker, int32 Target, int32 AttackType);

	static bool SendTopRankingRequest(); // ž�ٷ�ŷ�� �������� ������ ������ ��û �޼����� ������ �޼ҵ�

	static bool SendPassword(const FText& password, const int32& roomnumber);

	static bool SendSpawnOK();

	static bool SendMoveOK();

	static bool SendRevival(bool type);

	static bool SendBuyData(int32 Potion100, int32 Potion50, int32 TotalPrice);

	static bool SendRefreshItemData();

	static bool SendUsingItem(int32 type);


};
