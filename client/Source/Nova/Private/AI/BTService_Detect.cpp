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

		// OverlapMultiByChannel 반경내에 감지된 모든 캐릭터를 감지하는 함수
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
					//만약 범위 내의 캐릭터가 감지되면 타겟키 값에 해당 캐릭터 값을 넣어준다.
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ANovaMonsterAIController::TargetKey, ABCharacter);
					// 캐릭터를 감지하면 구체를 초록색으로 만든다.
					//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);

					// 감지한 캐릭터 위치에 파란색 점을 표시한다. 크기는 반지름 10짜리
					//DrawDebugPoint(World, ABCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);

					// 감지한 캐릭터와 NPC 사이에 파란색 라인을 그려준다.
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
					// 범위 내에 없다면 널포인트값을 넣어준다.
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
