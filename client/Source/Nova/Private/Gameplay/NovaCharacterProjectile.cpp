// Fill out your copyright notice in the Description page of Project Settings.

#include "NovaCharacterProjectile.h"
#include "NovaGameInstance.h"
#include "MonsterCharacter.h"


// Sets default values
ANovaCharacterProjectile::ANovaCharacterProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANovaCharacterProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANovaCharacterProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANovaCharacterProjectile::Skill_2_HitCheck(const FHitResult& result)
{
	if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
	{
		if (result.Actor.IsValid())
		{
			AMonsterCharacter* mon = Cast<AMonsterCharacter>(result.GetActor());
			if (mon != nullptr) {
				if (mon->IsMonsterAlive == true) {
					//UNovaGameInstance::NovaSocketcli->SendAttackInfo(Name, mon->Monsterid, 3); // 스킬 2번공격 3
					flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
					auto msg = object::CreateAttackInfo(builder,
						Name,
						mon->Monsterid,
						3);
					auto pk = object::CreatePacket(builder, object::Type_AttackInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

					builder.Finish(pk);

					uint8_t* send_buffer = builder.GetBufferPointer();

					// ---------
					int32 sent = 0;

					// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
					bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);

					ABLOG(Warning, TEXT("Hit Skill2 Monster Name : %d / Attacker : %d (IsAlive : %d(1=true, 0=false)"), mon->Monsterid, Name, mon->IsMonsterAlive);
				}
			}
		}
	}
}

void ANovaCharacterProjectile::WhoMakeThisProjectile(const int32 name)
{
	Name = name;
}

