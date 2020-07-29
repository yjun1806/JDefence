// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_Detect.h"
#include "NovaMonsterAIController.h"
#include "NovaCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "MonsterCharacter.h"
#include "NovaGameInstance.h"





UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;
	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f;

	auto mons = Cast<AMonsterCharacter>(ControllingPawn);
	if (mons->IsMonsterAlive == false) {
		OwnerComp.StopLogic("");
	}



	if (nullptr == World) return;

	if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode) {


		TArray<FOverlapResult> OverlapResults;
		FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

		// OverlapMultiByChannel �ݰ泻�� ������ ��� ĳ���͸� �����ϴ� �Լ�
		bool bResult = World->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity,
			ECollisionChannel::ECC_EngineTraceChannel2,
			FCollisionShape::MakeSphere(DetectRadius),
			CollisionQueryParam);


		if (bResult)
		{
			for (auto OverlapResult : OverlapResults)
			{
				ANovaCharacter* ABCharacter = Cast<ANovaCharacter>(OverlapResult.GetActor());
				//AFountain* Fountain = Cast<AFountain>(OverlapResult.GetActor());
				if (ABCharacter && ABCharacter->IsPlayerAlive == true)
				{
					//���� ���� ���� ĳ���Ͱ� �����Ǹ� Ÿ��Ű ���� �ش� ĳ���� ���� �־��ش�.
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ANovaMonsterAIController::TargetKey, ABCharacter);
					// ĳ���͸� �����ϸ� ��ü�� �ʷϻ����� �����.
					//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);

					// ������ ĳ���� ��ġ�� �Ķ��� ���� ǥ���Ѵ�. ũ��� ������ 10¥��
					//DrawDebugPoint(World, ABCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);

					// ������ ĳ���Ϳ� NPC ���̿� �Ķ��� ������ �׷��ش�.
					//DrawDebugLine(World, ControllingPawn->GetActorLocation(), ABCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
					return;
				}
				/*else if(Fountain && Fountain->IsFountainAlive == true)
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ANovaMonsterAIController::TargetKey, Fountain);
				}*/
				else
				{
					//ABLOG(Error, TEXT("BTService 2"));
					// ���� ���� ���ٸ� ������Ʈ���� �־��ش�.
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ANovaMonsterAIController::TargetKey, nullptr);
				}
			}
		}
	}
	/*else
	{
		auto mon = Cast<AMonsterCharacter>(ControllingPawn);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ANovaMonsterAIController::SameMonsterLocationKey, mon->NextLocation);
	}*/

	//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);

}
