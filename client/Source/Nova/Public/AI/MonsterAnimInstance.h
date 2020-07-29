// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	//UMonsterAnimInstance();
	//virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
		float CurrentPawnSpeed;
	
};
