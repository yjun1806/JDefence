// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_Attack.h"
#include "NovaMonsterAIController.h"
#include "MonsterCharacter.h"
#include "NovaGameInstance.h"




UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	IsAttaking = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	if (UNovaGameInstance::MyRoomInfo->RoomOwner != UNovaGameInstance::IDcode) return EBTNodeResult::Failed;


	auto ABCharacter = Cast<AMonsterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	// ���� NPC�� ���ٸ�(�׾��ٰų�..) Failed�� ��ȯ.
	if (nullptr == ABCharacter || ABCharacter->IsMonsterAlive == false) return EBTNodeResult::Failed;

	ABCharacter->Attack(); // ĳ������ Attack �Լ��� ȣ��
	IsAttaking = true; // ���� ���� ���̴�.

	// ������ ������ �Ʒ��� �����Լ��� ȣ���Ͽ� ������ �����ٰ� �ٲ��ش�.
	ABCharacter->OnAttackEnd.AddLambda([this]() -> void {
		IsAttaking = false;
		//ABLOG(Error, TEXT("IsAttacking? : %d"), IsAttaking);
	});

	// �켱�� InProgress�� ���ϰ��� ��ȯ�Ѵ�.
	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// ƽ �½�ũ�� ��� ȣ���Ͽ� ���������� �ƴ��� �Ǵ��Ѵ�.
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (UNovaGameInstance::MyRoomInfo->RoomOwner != UNovaGameInstance::IDcode) return;

	// ���� ������ �����ٸ�
	if (!IsAttaking)
	{
		// �½�ũ�� �����ٰ� �˷��ִ� �Լ��� ȣ���Ѵ�.
		// �� �Լ��� ȣ���� ���� ������ ���� �½�ũ�� ��� �ӹ��� �ȴ�.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
