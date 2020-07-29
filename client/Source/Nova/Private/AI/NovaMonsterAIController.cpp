// Fill out your copyright notice in the Description page of Project Settings.

#include "NovaMonsterAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NovaGameInstance.h"


const FName ANovaMonsterAIController::HomePosKey(TEXT("HomePos"));
const FName ANovaMonsterAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName ANovaMonsterAIController::TargetKey(TEXT("Target"));
const FName ANovaMonsterAIController::SameMonsterLocationKey(TEXT("SameMonsterLocation"));

ANovaMonsterAIController::ANovaMonsterAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT(
		"/Game/NovaProject/AI/BB_Monster.BB_Monster"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBCli(TEXT(
		"/Game/NovaProject/AI/BB_Client.BB_Client"));
	if (BBCli.Succeeded())
	{
		BBClient = BBCli.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT(
		"/Game/NovaProject/AI/BT_Monster.BT_Monster"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTCli(TEXT(
		"/Game/NovaProject/AI/BT_Client.BT_Client"));
	if (BTCli.Succeeded())
	{
		BTClient = BTCli.Object;
	}
}

void ANovaMonsterAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	//ABLOG(Error, TEXT("Room Owner : %s"), *UNovaGameInstance::MyRoomInfo.RoomOwner);
	if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode) {
		ABLOG(Error, TEXT("Tree Owner"));

		//GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AABAIController::OnRepeatTimer, RepeatInterval, true);
		if (UseBlackboard(BBAsset, Blackboard))
		{
			Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
			if (!RunBehaviorTree(BTAsset))
			{
				ABLOG(Error, TEXT("AIController couldn't run behavior Tree!"));
			}
		}
	}
	else {
		/*ABLOG(Error, TEXT("Tree Client"));

		if (UseBlackboard(BBClient, Blackboard))
		{
			Blackboard->SetValueAsVector(SameMonsterLocationKey, InPawn->GetActorLocation());
			if (!RunBehaviorTree(BTClient))
			{
				ABLOG(Error, TEXT("AIController couldn't run behavior Tree!"));
			}
		}*/
	}
}
