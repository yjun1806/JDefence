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
					//UNovaGameInstance::NovaSocketcli->SendAttackInfo(Name, mon->Monsterid, 3); // ��ų 2������ 3
					flatbuffers::FlatBufferBuilder builder(8192); // �����͸� ���� �÷����ۺ����� �����.
					auto msg = object::CreateAttackInfo(builder,
						Name,
						mon->Monsterid,
						3);
					auto pk = object::CreatePacket(builder, object::Type_AttackInfo, msg.Union()); // ��Ŷ�� �����ش�. �ݵ�� Ÿ�� ����!!

					builder.Finish(pk);

					uint8_t* send_buffer = builder.GetBufferPointer();

					// ---------
					int32 sent = 0;

					// ����� ���Ͽ� �����͸� ������, ������� �޴´�.
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

