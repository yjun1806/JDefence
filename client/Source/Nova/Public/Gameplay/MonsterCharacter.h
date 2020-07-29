// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "GameFramework/Character.h"
#include <string>
#include "MonsterCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);


UCLASS()
class NOVA_API AMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonsterCharacter();
	int32 Monsterid;
	int32 MonsterType;
	bool DiedAnim = false;

	UPROPERTY()
		bool MonsterMove = false;

	UPROPERTY(VisibleAnywhere, Category = "UI")
		class UWidgetComponent* MonsterHPBarWidget;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "ATTACK")
	bool IsAttacking;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "ATTACK")
		int32 AttackingCountNumber;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "STAT")
		float MonsterCurrentHP;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "STAT")
		float MonsterMaxHP;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "STAT")
		bool IsMonsterAlive;

	UFUNCTION(BlueprintImplementableEvent)
	void MoveMonster(const FVector& destination);

	UFUNCTION(BlueprintImplementableEvent)
		void MonsterDead();

	UFUNCTION()
		void MoveToLocation(const FVector &CharacterLocation);

	UPROPERTY()
		FVector NextLocation;

	FTimerHandle SendMonsterInfoHandler; // 캐릭터데이터를 서버에 전송하는 타이머 핸들러이다.

	void SendMonsterInfo();

	UFUNCTION(BlueprintImplementableEvent)
	void Attack();

	UFUNCTION(BlueprintCallable)
		void MonsterAttackCheck();

	UFUNCTION(BlueprintImplementableEvent)
		void AnotherPlayerAttack();

	UFUNCTION(BlueprintCallable, Category = "ATTACK")
		void SetAttack(bool att);

	UFUNCTION(BlueprintCallable, Category = "ATTACK")
		void SetAttackingNumber(int num);

	UFUNCTION(BlueprintCallable, Category = "ATTACK")
		bool GetIsAttack();

	FOnAttackEndDelegate OnAttackEnd;

	UFUNCTION()
		void UpdateMonsterHPbar();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	
	
};
