// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "flatbuffers/flatbuffers.h"
#include "Flatbuf/FlatPacket_generated.h"
#include "MessageDecode.generated.h"



/**
 * 
 */
UCLASS()
class NOVA_API UMessageDecode : public UObject
{
	GENERATED_BODY()
	
	
public:
	//void ObjectPacketDecoding(const object::Packet* objpacket);
	void ObjectPacketDecoding(TArray<uint8> Buffer);


	
};
