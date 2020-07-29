// Fill out your copyright notice in the Description page of Project Settings.

#include "NovaSocketClient.h"
#include <string>
#include <sstream>
#include <iostream>
#include "Networking.h"
#include "UObject/NoExportTypes.h"
#include "SocketSubsystem.h"
#include "Async.h"
#include "NovaGameInstance.h"
#include "Nova.h"
#include "Runtime/Sockets/Public/IPAddress.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"

#include "flatbuffers/flexbuffers.h"
#include "flatbuffers/flatbuffers.h"
#include "Flatbuf/FlatPacket_generated.h"
#include "Flatbuf/Login_generated.h"

using namespace std;


NovaSocketClient::NovaSocketClient()
{
	
}

NovaSocketClient::~NovaSocketClient()
{
	if (NovaSocket)
	{
		NovaSocket->Close();
		delete NovaSocket;
	}


}

FSocket* NovaSocketClient::NovaSocket = nullptr;

// ���� �ʱ�ȭ �κ�
bool NovaSocketClient::Init_Socket()
{
	// ������ �����ؼ� FSocket Ÿ���� �����Ϳ� �������ش�.
	NovaSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCP"), false);
	if (NovaSocket == 0) {
		return false;
	}

	return true;
}

// ������ �����ϴ� �κ�
bool NovaSocketClient::Connect_Server(int Port)
{
	//FIPv4Address ip(127, 0, 0, 1); // ���ü��� �ּ�
	FIPv4Address ip(54, 180, 102, 168); // AWS���� �ּ�

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	//addr->SetPort(1004);
	addr->SetPort(Port); // ���� ��Ʈ ����(���� 1004)

	return NovaSocket->Connect(*addr); // ���ῡ �����ϸ� true �����ϸ� false;
}

// ������������
bool NovaSocketClient::DisConnect_Server()
{
	bool result = false;
	if (NovaSocket)
	{
		result = NovaSocket->Close();
		delete NovaSocket;
	}
	return result;
}

//ȸ������
bool NovaSocketClient::Sign_up(const FText &Id, const FText &Pw)
{
	//================================================================================
	// ���� ����

	// FText ������ FString�������� ��ȯ
	FString ID = *Id.ToString();
	FString PASSWORD = *Pw.ToString();

	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto signup = object::CreateSignUpData(builder, // ������ ���ϴ� �����͸� ��´�. ���⿡�� �α��ε����͸� �����.
		builder.CreateString(std::string(TCHAR_TO_UTF8(*ID))), 
		builder.CreateString(std::string(TCHAR_TO_UTF8(*PASSWORD))));
	auto pk = object::CreatePacket(builder, object::Type_SignUpData, signup.Union()); // ��Ŷ�� �����ش�.

	builder.Finish(pk);
	
	uint8_t* send_buffer = builder.GetBufferPointer();

	int32 sent = 0;

	// �����͸� ������.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);
	//builder.Release();

	UE_LOG(LogTemp, Warning, TEXT("SignUp Send Result : [ %d ] (1: true, 0: false)"), nSendLend);
	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete &builder;
	//delete &signup;
	//delete &pk;
	//delete[] send_buffer; // ���۸� ������
	//send_buffer = NULL;

	// ���� ��
	//=================================================================================================
	// ���� ����

	// ������� ���� ������ ����
	TArray<uint8> ReceivedData;
	uint32 Size;

	bool SignUpResult = false;

	

	// HasPendingData ���Ͽ� �����Ͱ� �ִ��� ������ �Ǵ��ϴ� �Լ�, ������ true, ������ false;
	// �Ű������� recv ȣ�⿡ ���� �������� ���� ��Ÿ����.
	//NovaSocket->HasPendingData(Size);

	// �����͸� ���������� ��ٸ��ϴ�.
	NovaSocket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(3));
	//UE_LOG(LogTemp, Warning, TEXT("HasPendingData : %d"), NovaSocket->HasPendingData(Size));

	// ���Ͽ� �����Ͱ� �������� ���������� �д´�.
	while (NovaSocket->HasPendingData(Size)) {

		ReceivedData.Init(FMath::Min(Size, 65507u), Size);
		int32 Read = 0;

		// �����͸� �д� �κ�, ���� �����ʹ� ReceivedData�� �����Ѵ�.
		bool nRecv = NovaSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		UE_LOG(LogTemp, Warning, TEXT("SignUp Recv Result : %d"), nRecv);

		if (nRecv == false) {
			return false;
		}

		if (ReceivedData.Num() > 0) {
			UE_LOG(LogTemp, Warning, TEXT("Data Recv Result : %d"), ReceivedData.Num());

			// ���� �����͸� ���ۿ� ��´�.
			uint8_t* rbuffer = ReceivedData.GetData();
			size_t rSize = ReceivedData.Num();
			

			auto Packets = object::GetPacket(rbuffer);

			UE_LOG(LogTemp, Warning, TEXT("type %d"), Packets->packet_type_type());
			if (Packets->packet_type_type() == object::Type_SignUpData) 
			{
				auto sgd = reinterpret_cast<const object::SignUpData*>(Packets->packet_type());
				if (sgd->IsSignUpPossible() == true) {
				
					SignUpResult = true;
				}
				else

				{
					SignUpResult = false;
				}
			}
			else {

				return false;
			}

		}
		else
		{
			return false;
		}
		
	
	}

	return SignUpResult;
}

bool NovaSocketClient::Login(const FText &Id, const FText &Pw)
{
	//================================================================================
	// ���� ����

	// FText ������ FString�������� ��ȯ
	FString ID = *Id.ToString();
	FString PASSWORD = *Pw.ToString();
	// �÷����� �����κ�
	
	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto login = object::CreateLoginData(builder, // ����test�� ���ϴ� �����͸� ��´�. ���⿡�� �α��ε����͸� �����.
		builder.CreateString(std::string(TCHAR_TO_UTF8(*ID))),
		builder.CreateString(std::string(TCHAR_TO_UTF8(*PASSWORD))));
	auto pk = object::CreatePacket(builder, object::Type_LoginData, login.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);
	//builder.Release();


	UE_LOG(LogTemp, Warning, TEXT("Login Send Result : [ %d ] (1: true, 0: false)"), nSendLend);
	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete &builder;
	//delete &login;
	//delete &pk;
	//delete[] send_buffer; // ���۸� ������
	//send_buffer = NULL;

	// ���� ��
	//=================================================================================================
	// ���� ����

	// ������� ���� ������ ����
	TArray<uint8> ReceivedData;
	uint32 Size;

	bool LoginResult = false;

	// HasPendingData ���Ͽ� �����Ͱ� �ִ��� ������ �Ǵ��ϴ� �Լ�, ������ true, ������ false;
	// �Ű������� recv ȣ�⿡ ���� �������� ���� ��Ÿ����.

	// �����͸� ���������� ��ٸ��ϴ�.
	NovaSocket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(3));

	// ���Ͽ� �����Ͱ� �������� ���������� �д´�.
	while (NovaSocket->HasPendingData(Size)) {

		ReceivedData.Init(FMath::Min(Size, 65507u), Size);
		int32 Read = 0;

		// �����͸� �д� �κ�, ���� �����ʹ� ReceivedData�� �����Ѵ�.
		bool nRecv = NovaSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		UE_LOG(LogTemp, Warning, TEXT("Login Recv Result : %d"), nRecv);

		if (nRecv == false) {
			return false;
		}

		if (ReceivedData.Num() > 0) {
			UE_LOG(LogTemp, Warning, TEXT("Login Recv Result2 : %d"), ReceivedData.Num());

			// ���� �����͸� ���ۿ� ��´�.
			uint8_t* rbuffer = ReceivedData.GetData();
			
			auto packets = object::GetPacket(rbuffer);

			if (packets->packet_type_type() == object::Type_LoginData) { // �α��� �����͸� ���� ���
				auto lgd = reinterpret_cast<const object::LoginData*>(packets->packet_type());

				UE_LOG(LogTemp, Warning, TEXT("Data Recv Result2 : %d"), lgd->IsLoginOk());
				if (lgd->IsLoginOk() == true) { // �α��� ����!
					LoginResult = true;
					UNovaGameInstance::IDcode = lgd->IDCode(); // �������� ���� ���̵��ڵ尪�� �־��ش�.
					UNovaGameInstance::MyCurrentGold = lgd->Gold();
					UNovaGameInstance::CurrentPotion100 = lgd->Potion100();
					UNovaGameInstance::CurrentPotion50 = lgd->Potion50();
					break;
				}
				else //�α��� ����!
				{
					LoginResult = false;
				}
			}
			else // �ٸ� �����͸� ���� ���
			{
				return false;
			}

		}
		else
		{
			return false;
		}
	}

	return LoginResult;
}

bool NovaSocketClient::SendGameState(int32 code)
{
	UE_LOG(LogTemp, Warning, TEXT("SendGameState : %d"), code);
	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto login = object::CreateGameState(builder, // ������ ���ϴ� �����͸� ��´�. ���⿡�� �α��ε����͸� �����.
		code,
		UNovaGameInstance::IDcode);
	auto pk = object::CreatePacket(builder, object::Type_GameState, login.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		UE_LOG(LogTemp, Warning, TEXT("SendFail"));

		return false;
	}

	//delete[] send_buffer; // ���۸� ������
	//builder.ReleaseBufferPointer();
	return nSendLend;
}
//
//// �� �Լ��� �Ѱܹ��� ������Ʈ��Ŷ�� ��Ʈ������ ��ȯ���Ѽ� ���Ͽ� ��� �����ִ� ������ �Ѵ�.
//bool NovaSocketClient::SendObjectPacket(ObjectPacket &sendpk)
//{
	//sendpk.set_gamestatecode(UNovaGameInstance::NowPlayStateCode);
	//sendpk.set_clientid(UNovaGameInstance::userLoginID);

	//size_t pSize = sendpk.ByteSize() + 5; // +5�� ���ϴ°ɱ�? ���ϸ� �ȵǳ�??

	//uint8_t* buffer = new uint8_t[pSize]; // �������� ���� �����͸� ���� ���۸� �����Ѵ�. uint8_t : 8��Ʈ ũ���� ��ȣ�� ���� ������(0~255) ����Ÿ��(����� %u��)
	//google::protobuf::io::ArrayOutputStream arr(buffer, pSize); // 
	//google::protobuf::io::CodedOutputStream output(&arr);

	////  <google/protobuf/io/coded_stream.h> �� ����� ���Ե� �Լ��̴�.
	//output.WriteVarint32(sendpk.ByteSize());
	//sendpk.SerializeToCodedStream(&output);

	//int32 sent = 0;
	//bool nSendLend = NovaSocket->Send(buffer, output.ByteCount(), sent);

	//delete[] buffer;
	////UE_LOG(LogTemp, Warning, TEXT("ObjectPacket Send Count : [ %d ] (1: Success!, 0: Faild!)"), output.ByteCount());

	//return nSendLend;
//}
//
//
bool NovaSocketClient::SendMessageToServer(const FString &text, int32 gamestatecode, string UserID)
{
	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateChatMessage(builder, // ������ ���ϴ� �����͸� ��´�. ���⿡�� �α��ε����͸� �����.
		builder.CreateString(UserID),
		builder.CreateString(std::string(TCHAR_TO_UTF8(*text))),
			gamestatecode);
	auto pk = object::CreatePacket(builder, object::Type_ChatMessage, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
	
}
//
//
//bool NovaSocketClient::ReadDelimitedFrom(google::protobuf::io::CodedInputStream * input, google::protobuf::MessageLite * message)
//{
//	// Read the size.
//	uint32_t size;
//	if (!input->ReadVarint32(&size)) return false;
//	//UE_LOG(LogTemp, Warning, TEXT("Delim : [ %d ]"), size);
//
//	// Tell the stream not to read beyond that size.
//	google::protobuf::io::CodedInputStream::Limit limit = input->PushLimit(size);
//
//	// Parse the message.
//	if (!message->MergeFromCodedStream(input)) return false;
//	if (!input->ConsumedEntireMessage()) return false;
//
//	// Release the limit.
//	input->PopLimit(limit);
//
//	return true;
//}
//
////ĳ���� �����͸� �����ϴ� �κ�
//bool NovaSocketClient::SendCharaterState(PlayerData* data)
//{
//	std::shared_ptr<ObjectPacket> obj(new ObjectPacket);
//	obj->set_allocated_playerdata(data);
//
//	bool sendSuccess = SendObjectPacket(*obj.get());
//	//UE_LOG(LogTemp, Warning, TEXT("IsSuccessful? : %d"), sendSuccess);
//
//	if (obj->has_playerdata()) {
//		obj->release_playerdata();
//	}
//
//	return sendSuccess;
//}
//
//bool NovaSocketClient::SendMonsterData(MonsterData* data)
//{
//	std::shared_ptr<ObjectPacket> obj(new ObjectPacket);
//	obj->set_allocated_monsterdata(data);
//
//	bool sendSuccess = SendObjectPacket(*obj.get());
//	//UE_LOG(LogTemp, Warning, TEXT("IsSuccessful? : %d"), sendSuccess);
//
//	if (obj->has_monsterdata()) {
//		obj->release_monsterdata();
//	}
//
//	return sendSuccess;
//}
//
bool NovaSocketClient::SendRoomInfo()
{
	
	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateRoomInfo(builder,
		builder.CreateString(UNovaGameInstance::MyRoomInfo->RoomName),
		UNovaGameInstance::MyRoomInfo->RoomNumber,
		UNovaGameInstance::MyRoomInfo->MaxUserNumber,
		UNovaGameInstance::MyRoomInfo->CurrentUserNumber,
		builder.CreateString(UNovaGameInstance::MyRoomInfo->Password),
		UNovaGameInstance::MyRoomInfo->RoomOwner,
		UNovaGameInstance::MyRoomInfo->IsThisRoomPlayingGame,
		0,
		UNovaGameInstance::MyRoomInfo->FountainHealth,
		UNovaGameInstance::MyRoomInfo->IsThisRoomLock);
	auto pk = object::CreatePacket(builder, object::Type_RoomInfo, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendGameReadyInfo(int32 roomnumber, bool isready)
{

	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateGameReadyInfo(builder,
		UNovaGameInstance::IDcode,
		isready,
		roomnumber);
	auto pk = object::CreatePacket(builder, object::Type_GameReadyInfo, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendLeaveRoomInfo(int32 roomnumber)
{
	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateLeaveRoom(builder,
		UNovaGameInstance::IDcode,
		roomnumber);
	auto pk = object::CreatePacket(builder, object::Type_LeaveRoom, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendGameStartInfo(int32 roomnumber, bool isstart)
{
	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateGameStartInfo(builder,
		roomnumber,
		isstart,false,0,false);
	auto pk = object::CreatePacket(builder, object::Type_GameStartInfo, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendGameLoadingComplete(int32 roomnumber, bool isloadingcomplete)
{
	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateGameStartInfo(builder,
		roomnumber,
		true, isloadingcomplete, 0, false);
	auto pk = object::CreatePacket(builder, object::Type_GameStartInfo, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;

}

bool NovaSocketClient::SendAttackInfo(int32 Attacker, int32 Target, int32 AttackType)
{
	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateAttackInfo(builder,
		Attacker,
		Target,
		AttackType);
	auto pk = object::CreatePacket(builder, object::Type_AttackInfo, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendTopRankingRequest()
{
	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateTopRanking(builder,
		true);
	auto pk = object::CreatePacket(builder, object::Type_TopRanking, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!



	// ������ �κ��� ���� �մ�������
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendPassword(const FText & password, const int32& roomnumber)
{
	FString ps = *password.ToString();

	flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateCheckPassword(builder,
		builder.CreateString(std::string(TCHAR_TO_UTF8(*ps))),
		false,
		roomnumber);
	auto pk = object::CreatePacket(builder, object::Type_CheckPassword, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	// ������ �κ��� ���� �մ�������
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendSpawnOK()
{
	flatbuffers::FlatBufferBuilder builder(1024); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateSpawnOK(builder,
		true);
	auto pk = object::CreatePacket(builder, object::Type_SpawnOK, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	// ������ �κ��� ���� �մ�������
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendMoveOK()
{
	flatbuffers::FlatBufferBuilder builder(1024); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateMoveOK(builder,
		true);
	auto pk = object::CreatePacket(builder, object::Type_MoveOK, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	// ������ �κ��� ���� �մ�������
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendRevival(bool type)
{
	flatbuffers::FlatBufferBuilder builder(1024); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateRevival(builder,
		type);
	auto pk = object::CreatePacket(builder, object::Type_Revival, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	// ������ �κ��� ���� �մ�������
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendBuyData(int32 Potion100, int32 Potion50, int32 TotalPrice)
{
	flatbuffers::FlatBufferBuilder builder(1024); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateItemBuy(builder,
		Potion100, Potion50, TotalPrice);
	auto pk = object::CreatePacket(builder, object::Type_ItemBuy, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	// ������ �κ��� ���� �մ�������
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendRefreshItemData()
{
	flatbuffers::FlatBufferBuilder builder(1024); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateRefreshItem(builder, 0, 0, 0);
	auto pk = object::CreatePacket(builder, object::Type_RefreshItem, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	// ������ �κ��� ���� �մ�������
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}

bool NovaSocketClient::SendUsingItem(int32 type)
{
	flatbuffers::FlatBufferBuilder builder(1024); // �����͸� ���� �÷����ۺ����� �����.
	auto msg = object::CreateItemUsing(builder, type, UNovaGameInstance::IDcode);
	auto pk = object::CreatePacket(builder, object::Type_ItemUsing, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

	// ������ �κ��� ���� �մ�������
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // ������ ���� ����
		return false;
	}

	//delete[] send_buffer; // ���۸� ������

	return nSendLend;
}
