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

// 소켓 초기화 부분
bool NovaSocketClient::Init_Socket()
{
	// 소켓을 생성해서 FSocket 타입의 포인터에 연결해준다.
	NovaSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCP"), false);
	if (NovaSocket == 0) {
		return false;
	}

	return true;
}

// 서버와 연결하는 부분
bool NovaSocketClient::Connect_Server(int Port)
{
	//FIPv4Address ip(127, 0, 0, 1); // 로컬서버 주소
	FIPv4Address ip(54, 180, 102, 168); // AWS서버 주소

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	//addr->SetPort(1004);
	addr->SetPort(Port); // 서버 포트 설정(현재 1004)

	return NovaSocket->Connect(*addr); // 연결에 성공하면 true 실패하면 false;
}

// 서버연결해제
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

//회원가입
bool NovaSocketClient::Sign_up(const FText &Id, const FText &Pw)
{
	//================================================================================
	// 전송 시작

	// FText 형식을 FString형식으로 변환
	FString ID = *Id.ToString();
	FString PASSWORD = *Pw.ToString();

	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto signup = object::CreateSignUpData(builder, // 빌더에 원하는 데이터를 담는다. 여기에선 로그인데이터를 만든다.
		builder.CreateString(std::string(TCHAR_TO_UTF8(*ID))), 
		builder.CreateString(std::string(TCHAR_TO_UTF8(*PASSWORD))));
	auto pk = object::CreatePacket(builder, object::Type_SignUpData, signup.Union()); // 패킷에 묶어준다.

	builder.Finish(pk);
	
	uint8_t* send_buffer = builder.GetBufferPointer();

	int32 sent = 0;

	// 데이터를 보낸다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);
	//builder.Release();

	UE_LOG(LogTemp, Warning, TEXT("SignUp Send Result : [ %d ] (1: true, 0: false)"), nSendLend);
	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete &builder;
	//delete &signup;
	//delete &pk;
	//delete[] send_buffer; // 버퍼를 지워줌
	//send_buffer = NULL;

	// 전송 끝
	//=================================================================================================
	// 수신 시작

	// 결과값을 담을 변수들 선언
	TArray<uint8> ReceivedData;
	uint32 Size;

	bool SignUpResult = false;

	

	// HasPendingData 소켓에 데이터가 있는지 없는지 판단하는 함수, 있으면 true, 없으면 false;
	// 매개변수는 recv 호출에 대한 데이터의 양을 나타낸다.
	//NovaSocket->HasPendingData(Size);

	// 데이터를 읽을떄까지 기다립니다.
	NovaSocket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(3));
	//UE_LOG(LogTemp, Warning, TEXT("HasPendingData : %d"), NovaSocket->HasPendingData(Size));

	// 소켓에 데이터가 남아있지 않을때까지 읽는다.
	while (NovaSocket->HasPendingData(Size)) {

		ReceivedData.Init(FMath::Min(Size, 65507u), Size);
		int32 Read = 0;

		// 데이터를 읽는 부분, 읽은 데이터는 ReceivedData에 저장한다.
		bool nRecv = NovaSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		UE_LOG(LogTemp, Warning, TEXT("SignUp Recv Result : %d"), nRecv);

		if (nRecv == false) {
			return false;
		}

		if (ReceivedData.Num() > 0) {
			UE_LOG(LogTemp, Warning, TEXT("Data Recv Result : %d"), ReceivedData.Num());

			// 받은 데이터를 버퍼에 담는다.
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
	// 전송 시작

	// FText 형식을 FString형식으로 변환
	FString ID = *Id.ToString();
	FString PASSWORD = *Pw.ToString();
	// 플랫버퍼 생성부분
	
	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto login = object::CreateLoginData(builder, // 빌더test에 원하는 데이터를 담는다. 여기에선 로그인데이터를 만든다.
		builder.CreateString(std::string(TCHAR_TO_UTF8(*ID))),
		builder.CreateString(std::string(TCHAR_TO_UTF8(*PASSWORD))));
	auto pk = object::CreatePacket(builder, object::Type_LoginData, login.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);
	//builder.Release();


	UE_LOG(LogTemp, Warning, TEXT("Login Send Result : [ %d ] (1: true, 0: false)"), nSendLend);
	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete &builder;
	//delete &login;
	//delete &pk;
	//delete[] send_buffer; // 버퍼를 지워줌
	//send_buffer = NULL;

	// 전송 끝
	//=================================================================================================
	// 수신 시작

	// 결과값을 담을 변수들 선언
	TArray<uint8> ReceivedData;
	uint32 Size;

	bool LoginResult = false;

	// HasPendingData 소켓에 데이터가 있는지 없는지 판단하는 함수, 있으면 true, 없으면 false;
	// 매개변수는 recv 호출에 대한 데이터의 양을 나타낸다.

	// 데이터를 읽을떄까지 기다립니다.
	NovaSocket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(3));

	// 소켓에 데이터가 남아있지 않을때까지 읽는다.
	while (NovaSocket->HasPendingData(Size)) {

		ReceivedData.Init(FMath::Min(Size, 65507u), Size);
		int32 Read = 0;

		// 데이터를 읽는 부분, 읽은 데이터는 ReceivedData에 저장한다.
		bool nRecv = NovaSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		UE_LOG(LogTemp, Warning, TEXT("Login Recv Result : %d"), nRecv);

		if (nRecv == false) {
			return false;
		}

		if (ReceivedData.Num() > 0) {
			UE_LOG(LogTemp, Warning, TEXT("Login Recv Result2 : %d"), ReceivedData.Num());

			// 받은 데이터를 버퍼에 담는다.
			uint8_t* rbuffer = ReceivedData.GetData();
			
			auto packets = object::GetPacket(rbuffer);

			if (packets->packet_type_type() == object::Type_LoginData) { // 로그인 데이터를 받은 경우
				auto lgd = reinterpret_cast<const object::LoginData*>(packets->packet_type());

				UE_LOG(LogTemp, Warning, TEXT("Data Recv Result2 : %d"), lgd->IsLoginOk());
				if (lgd->IsLoginOk() == true) { // 로그인 성공!
					LoginResult = true;
					UNovaGameInstance::IDcode = lgd->IDCode(); // 서버에서 받은 아이디코드값을 넣어준다.
					UNovaGameInstance::MyCurrentGold = lgd->Gold();
					UNovaGameInstance::CurrentPotion100 = lgd->Potion100();
					UNovaGameInstance::CurrentPotion50 = lgd->Potion50();
					break;
				}
				else //로그인 실패!
				{
					LoginResult = false;
				}
			}
			else // 다른 데이터를 받은 경우
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
	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto login = object::CreateGameState(builder, // 빌더에 원하는 데이터를 담는다. 여기에선 로그인데이터를 만든다.
		code,
		UNovaGameInstance::IDcode);
	auto pk = object::CreatePacket(builder, object::Type_GameState, login.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		UE_LOG(LogTemp, Warning, TEXT("SendFail"));

		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌
	//builder.ReleaseBufferPointer();
	return nSendLend;
}
//
//// 이 함수는 넘겨받은 오브젝트패킷을 스트림으로 변환시켜서 소켓에 담아 보내주는 역할을 한다.
//bool NovaSocketClient::SendObjectPacket(ObjectPacket &sendpk)
//{
	//sendpk.set_gamestatecode(UNovaGameInstance::NowPlayStateCode);
	//sendpk.set_clientid(UNovaGameInstance::userLoginID);

	//size_t pSize = sendpk.ByteSize() + 5; // +5는 왜하는걸까? 안하면 안되나??

	//uint8_t* buffer = new uint8_t[pSize]; // 소켓으로 보낼 데이터를 담을 버퍼를 선언한다. uint8_t : 8비트 크기의 부호가 없는 정수형(0~255) 변수타입(출력은 %u로)
	//google::protobuf::io::ArrayOutputStream arr(buffer, pSize); // 
	//google::protobuf::io::CodedOutputStream output(&arr);

	////  <google/protobuf/io/coded_stream.h> 이 헤더에 포함된 함수이다.
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
	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateChatMessage(builder, // 빌더에 원하는 데이터를 담는다. 여기에선 로그인데이터를 만든다.
		builder.CreateString(UserID),
		builder.CreateString(std::string(TCHAR_TO_UTF8(*text))),
			gamestatecode);
	auto pk = object::CreatePacket(builder, object::Type_ChatMessage, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

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
////캐릭터 데이터를 전송하는 부분
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
	
	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
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
	auto pk = object::CreatePacket(builder, object::Type_RoomInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendGameReadyInfo(int32 roomnumber, bool isready)
{

	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateGameReadyInfo(builder,
		UNovaGameInstance::IDcode,
		isready,
		roomnumber);
	auto pk = object::CreatePacket(builder, object::Type_GameReadyInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendLeaveRoomInfo(int32 roomnumber)
{
	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateLeaveRoom(builder,
		UNovaGameInstance::IDcode,
		roomnumber);
	auto pk = object::CreatePacket(builder, object::Type_LeaveRoom, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendGameStartInfo(int32 roomnumber, bool isstart)
{
	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateGameStartInfo(builder,
		roomnumber,
		isstart,false,0,false);
	auto pk = object::CreatePacket(builder, object::Type_GameStartInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendGameLoadingComplete(int32 roomnumber, bool isloadingcomplete)
{
	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateGameStartInfo(builder,
		roomnumber,
		true, isloadingcomplete, 0, false);
	auto pk = object::CreatePacket(builder, object::Type_GameStartInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;

}

bool NovaSocketClient::SendAttackInfo(int32 Attacker, int32 Target, int32 AttackType)
{
	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateAttackInfo(builder,
		Attacker,
		Target,
		AttackType);
	auto pk = object::CreatePacket(builder, object::Type_AttackInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	// ---------
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendTopRankingRequest()
{
	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateTopRanking(builder,
		true);
	auto pk = object::CreatePacket(builder, object::Type_TopRanking, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!



	// 나머지 부분은 공통 손대지말자
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendPassword(const FText & password, const int32& roomnumber)
{
	FString ps = *password.ToString();

	flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateCheckPassword(builder,
		builder.CreateString(std::string(TCHAR_TO_UTF8(*ps))),
		false,
		roomnumber);
	auto pk = object::CreatePacket(builder, object::Type_CheckPassword, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	// 나머지 부분은 공통 손대지말자
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendSpawnOK()
{
	flatbuffers::FlatBufferBuilder builder(1024); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateSpawnOK(builder,
		true);
	auto pk = object::CreatePacket(builder, object::Type_SpawnOK, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	// 나머지 부분은 공통 손대지말자
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendMoveOK()
{
	flatbuffers::FlatBufferBuilder builder(1024); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateMoveOK(builder,
		true);
	auto pk = object::CreatePacket(builder, object::Type_MoveOK, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	// 나머지 부분은 공통 손대지말자
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendRevival(bool type)
{
	flatbuffers::FlatBufferBuilder builder(1024); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateRevival(builder,
		type);
	auto pk = object::CreatePacket(builder, object::Type_Revival, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	// 나머지 부분은 공통 손대지말자
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendBuyData(int32 Potion100, int32 Potion50, int32 TotalPrice)
{
	flatbuffers::FlatBufferBuilder builder(1024); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateItemBuy(builder,
		Potion100, Potion50, TotalPrice);
	auto pk = object::CreatePacket(builder, object::Type_ItemBuy, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	// 나머지 부분은 공통 손대지말자
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendRefreshItemData()
{
	flatbuffers::FlatBufferBuilder builder(1024); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateRefreshItem(builder, 0, 0, 0);
	auto pk = object::CreatePacket(builder, object::Type_RefreshItem, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	// 나머지 부분은 공통 손대지말자
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}

bool NovaSocketClient::SendUsingItem(int32 type)
{
	flatbuffers::FlatBufferBuilder builder(1024); // 데이터를 담을 플랫버퍼빌더를 만든다.
	auto msg = object::CreateItemUsing(builder, type, UNovaGameInstance::IDcode);
	auto pk = object::CreatePacket(builder, object::Type_ItemUsing, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

	// 나머지 부분은 공통 손대지말자
	builder.Finish(pk);
	uint8_t* send_buffer = builder.GetBufferPointer();
	int32 sent = 0;

	// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
	bool nSendLend = NovaSocket->Send(send_buffer, builder.GetSize(), sent);

	if (nSendLend == false) { // 데이터 전송 실패
		return false;
	}

	//delete[] send_buffer; // 버퍼를 지워줌

	return nSendLend;
}
