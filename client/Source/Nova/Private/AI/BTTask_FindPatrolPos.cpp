// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindPatrolPos.h"
#include "NovaMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NovaGameInstance.h"
#include "MonsterCharacter.h"
//#include "NavigationSystem.h"



UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
	
	FindIndex = 0;

}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed; // 태스크 수행에 실패한 경우
	}


	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(ANovaMonsterAIController::HomePosKey);

	auto mon = Cast<AMonsterCharacter>(ControllingPawn);
	if (mon->MonsterMove == false) {
		return EBTNodeResult::Failed;
	}

	if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode) {
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ANovaMonsterAIController::PatrolPosKey, FVector(40.0f, 670.0f, 120.0f)); // 다른맵
		//OwnerComp.GetBlackboardComponent()->SetValueAsVector(ANovaMonsterAIController::PatrolPosKey, FVector(-5700.0f, 5680.0f, 7120.0f)); // 기본맵
		//OwnerComp.GetBlackboardComponent()->SetValueAsVector(ANovaMonsterAIController::PatrolPosKey, FVector(-2870.0f, 280.0f, 8140.0f)); // 기본맵
		return EBTNodeResult::Succeeded;
	}
	

	return EBTNodeResult::Failed;
}
