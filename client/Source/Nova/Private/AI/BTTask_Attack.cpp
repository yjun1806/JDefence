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
	// 만약 NPC가 없다면(죽었다거나..) Failed를 반환.
	if (nullptr == ABCharacter || ABCharacter->IsMonsterAlive == false) return EBTNodeResult::Failed;

	ABCharacter->Attack(); // 캐릭터의 Attack 함수를 호출
	IsAttaking = true; // 현재 공격 중이다.

	// 공격이 끝나면 아래의 람다함수를 호출하여 공격이 끝났다고 바꿔준다.
	ABCharacter->OnAttackEnd.AddLambda([this]() -> void {
		IsAttaking = false;
		//ABLOG(Error, TEXT("IsAttacking? : %d"), IsAttaking);
	});

	// 우선은 InProgress로 리턴값을 반환한다.
	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 틱 태스크를 계속 호출하여 공격중인지 아닌지 판단한다.
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (UNovaGameInstance::MyRoomInfo->RoomOwner != UNovaGameInstance::IDcode) return;

	// 만약 공격이 끝났다면
	if (!IsAttaking)
	{
		// 태스크가 끝났다고 알려주는 함수를 호출한다.
		// 이 함수를 호출해 주지 않으면 공격 태스크에 계속 머물게 된다.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
