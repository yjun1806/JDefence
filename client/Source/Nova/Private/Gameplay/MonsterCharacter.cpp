// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterCharacter.h"
#include "NovaGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "NovaMonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "MonsterHPWidget.h"
#include "Engine/World.h"





// Sets default values
AMonsterCharacter::AMonsterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	//GetActor()->SetActorScale3D(FVector(2.0f, 2.0f, 2.0f));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));

	// AI��Ʈ�ѷ� ����
	AIControllerClass = ANovaMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;



	AttackingCountNumber = -1;
	IsAttacking = false;
	MonsterType = 1;
	IsMonsterAlive = true;
	Monsterid = 0;

}



void AMonsterCharacter::MoveToLocation(const FVector &CharacterLocation)
{
	ANovaMonsterAIController* Controller = Cast<ANovaMonsterAIController>(GetController());
	if (Controller)
	{
		Controller->MoveToLocation(CharacterLocation);
	}
}

void AMonsterCharacter::SendMonsterInfo()
{
	auto Character_cli = Cast<AMonsterCharacter>(this);
	if (!Character_cli) return;

	

	// ������ ���� ĳ���� ������ �ۼ�
	flatbuffers::FlatBufferBuilder builder(8192); // �÷����� ����
	auto Location = object::CreateLocation(builder, // �����̼ǰ� ����
		Character_cli->GetActorLocation().X,
		Character_cli->GetActorLocation().Y,
		Character_cli->GetActorLocation().Z);

	auto pd = object::CreateMonsterData(builder, // �÷��̾� ������ �� �ۼ�
		Monsterid,
		0, //������ ���� ü���� ������ ������ �ʴ´�. �ֳĸ� ��� ü�°� ������ ���� ó���� �������� �ϱ�����
		Location,
		IsMonsterAlive,
		IsAttacking,
		AttackingCountNumber);
	auto pk = object::CreatePacket(builder, object::Type_MonsterData, pd.Union()); // ��Ŷ���� �����ش�.

	builder.Finish(pk);

	uint8_t* send_buffer = builder.GetBufferPointer();

	int32 sent = 0;

	// �����͸� ������.
	bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);
	//builder.Release();

}


void AMonsterCharacter::MonsterAttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	float range = 200.0f;
	float sphere = 50.0f;
	if (MonsterType == 3) {
		range = 400.0f;
		sphere = 150.0f;
	}
	else if (MonsterType == 2) {
		range = 300.0f;
		sphere = 100.0f;
	}

	bool bResult = GetWorld()->SweepSingleByChannel(HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector()*range,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel4, // ĳ���� ���� ä�� 4��
		FCollisionShape::MakeSphere(sphere),
		Params);
	//ABLOG(Warning, TEXT("AttackCheck : %d"), MonsterType);


	if (bResult && UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
	{
		//ABLOG(Warning, TEXT("AttackCheck2 : %d"), MonsterType);

		if (HitResult.Actor.IsValid())
		{
			//ABLOG(Warning, TEXT("AttackCheck3 : %d"), MonsterType);
			ANovaCharacter* nch = Cast<ANovaCharacter>(HitResult.GetActor());
			if (nch != nullptr) {
				if (nch->IsPlayerAlive == true) {
					if (MonsterType == 1) {
						UNovaGameInstance::NovaSocketcli->SendAttackInfo(Monsterid, nch->CharacterID, 10); // ������ ������ ���, ����Ÿ�� 10 : �Ϲݸ��� ����
						ABLOG(Warning, TEXT("NormalAttack to -> %d"), nch->CharacterID);
					}
					else if (MonsterType == 2) {
						UNovaGameInstance::NovaSocketcli->SendAttackInfo(Monsterid, nch->CharacterID, 11); // ������ ������ ���, ����Ÿ�� 11 : ������� ����
						ABLOG(Warning, TEXT("RareAttack to -> %d"), nch->CharacterID);

					}
					else if (MonsterType == 3) {
						ABLOG(Warning, TEXT("BossAttack to -> %d"), nch->CharacterID);
						UNovaGameInstance::NovaSocketcli->SendAttackInfo(Monsterid, nch->CharacterID, 12); // ������ ������ ���, ����Ÿ�� 12 : �������� ����

					}
					//ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());
				}
			}
		}
	}
}

void AMonsterCharacter::SetAttack(bool att)
{
	IsAttacking = att;
}

void AMonsterCharacter::SetAttackingNumber(int num)
{
	AttackingCountNumber = num;
}

bool AMonsterCharacter::GetIsAttack()
{
	return IsAttacking;
}

void AMonsterCharacter::UpdateMonsterHPbar()
{
	auto bar = Cast<UMonsterHPWidget>(MonsterHPBarWidget->GetUserWidgetObject());
	if (bar != nullptr)
	{
		//ABLOG(Error, TEXT("why why why %f, %f"), MonsterCurrentHP, MonsterMaxHP);

		bar->UpdateProgressBar(this);
	}
}



// Called when the game starts or when spawned
void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode) {
		GetWorldTimerManager().SetTimer(SendMonsterInfoHandler, this, &AMonsterCharacter::SendMonsterInfo, 0.016f, true);
		//UE_LOG(LogTemp, Error, TEXT("MonsterDataSendStart : %s"), *UNovaGameInstance::MyRoomInfo.RoomOwner);
	}


	
}

// Called every frame
void AMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MonsterCurrentHP <= 0.0f && DiedAnim == false)
	{
		MonsterDead();
		DiedAnim = true;
	}

	if (GetActorLocation().Z < -1000.0f)
	{
		int ID = Monsterid;
		if (Destroy()) {
			UNovaGameInstance::MonsterList.Remove(ID);
		}
	}

}

// Called to bind functionality to input
void AMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

