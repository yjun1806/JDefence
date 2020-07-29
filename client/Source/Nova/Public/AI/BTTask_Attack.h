// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Attack();

	// 공격 태스크는 공격 애니메이션이 끝날 때까지 대기하야 하는 지연 태스크이므로
	// ExecuteTask의 결과 값을 InProgress로 일단 반환하고
	// 공격 애니메이션이 끝나면 끝났다고 알려줘야 한다.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsAttaking = false;
	
	
};
