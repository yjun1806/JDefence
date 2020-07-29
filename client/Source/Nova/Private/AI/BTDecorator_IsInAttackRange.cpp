// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_IsInAttackRange.h"
#include "NovaMonsterAIController.h"
#include "MonsterCharacter.h"
#include "NovaCharacter.h"
#include "NovaGameInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <string>

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (UNovaGameInstance::MyRoomInfo->RoomOwner != UNovaGameInstance::IDcode) return false;

	// OwnerComp �� ���ڷ����͸� �����ϰ� �ִ� ������Ʈ(?) : AI ���Ǿ��� �ǰڴ�.
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	//auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	auto ControllingPawn = Cast<AMonsterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn) return false;

	auto Target = Cast<ANovaCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ANovaMonsterAIController::TargetKey));
	//auto Target2 = Cast<AFountain>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ANovaMonsterAIController::TargetKey));
	if (nullptr == Target) return false;

	int32  towerID = 999;
	if (Target->CharacterID == towerID) {
		bResult = (Target->GetDistanceTo(ControllingPawn) <= 450.0f);
		//ABLOG(Warning, TEXT("Turret : %f"), Target->GetDistanceTo(ControllingPawn));
	}
	else {
		bResult = (Target->GetDistanceTo(ControllingPawn) <= 300.0f);
		//ABLOG(Warning, TEXT("Character : %f"), Target->GetDistanceTo(ControllingPawn));

	}

	//Ÿ�ٰ� NPC ������ �Ÿ��� NPC�� ���ݹ������� ������ �ƴ���
	
	//bool bResult2 = (Target2->GetDistanceTo(ControllingPawn) <= 200.0f);

	/*bool FinalResult = false;

	if (bResult || bResult2) {
		FinalResult = true;
	}*/
	//ABLOG(Error, TEXT("bResult : %d"), bResult);

	// �����ȿ� ������ true, �ƴϸ� false


	return bResult;
}
