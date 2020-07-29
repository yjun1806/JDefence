// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "AIController.h"
#include "NovaMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API ANovaMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ANovaMonsterAIController();
	virtual void Possess(APawn* InPawn) override;
	
	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
	static const FName SameMonsterLocationKey;


private:
	UPROPERTY()
		class UBehaviorTree* BTAsset;

	UPROPERTY()
		class UBlackboardData* BBAsset;

	UPROPERTY()
		class UBehaviorTree* BTClient;

	UPROPERTY()
		class UBlackboardData* BBClient;
	
};
