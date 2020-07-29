// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Nova.h"
#include <string>
//#include "NovaCharacterHpBarWidget.h"
#include "GameFramework/Character.h"
#include "NovaCharacter.generated.h"

UCLASS(config=Game)
class ANovaCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/*UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* HPBarWidget;*/

public:
	ANovaCharacter();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;


	// 캐릭터 아이디
	int32 CharacterID;
	std::string CharacterName;

	//UNovaCharacterHpBarWidget* hpbarwiget;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "ATTACK")
		int32 AttackingCountNumber;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "ATTACK")
		bool IsAttacking;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "STAT")
	float CharacterCurrentHP;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "STAT")
	float CharacterMaxHP;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "STAT")
		bool IsPlayerAlive;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "STAT")
		bool IsSkillUsing;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "STAT")
		int32 SkillUsingNumber;

	UFUNCTION(BlueprintCallable)
		void SetCharacterName(const int32 name);


	UFUNCTION(BlueprintCallable, Category = "ATTACK")
		void SetAttackingNumber(int32 number);

	UFUNCTION(BlueprintCallable, Category = "ATTACK")
		void SetIsAttacking(bool boo);

	UFUNCTION()
		void UpdateCharacterHPbar();

	// 캐릭터 죽었을때 애니메이션 처리 ->블루프린트에서
	UFUNCTION(BlueprintImplementableEvent)
		void CharacterDead();

	// 캐릭터 부활처리
	UFUNCTION(BlueprintImplementableEvent)
		void CharacterRevival();

protected:



	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Attack();

	UFUNCTION(BlueprintImplementableEvent)
	void AnotherPlayerAttack();

	UFUNCTION(BlueprintCallable)
		void AttackCheck();

	UFUNCTION(BlueprintCallable)
		void Skill_1_Check(const FHitResult& result);

	UPROPERTY(BlueprintReadWrite, Category = "Skill")
		bool IsSkill_1_CoolDown;

	UPROPERTY(BlueprintReadWrite, Category = "Skill")
		bool IsSkill_2_CoolDown;

	UFUNCTION(BlueprintImplementableEvent)
		void Skill_1_Using();

	UFUNCTION(BlueprintImplementableEvent)
		void Skill_2_Using();


	UFUNCTION(BlueprintImplementableEvent)
		void AnotherPlayer_SkillUsing();

	UFUNCTION(BlueprintCallable)
		int32 GetCharacterID();

private:
	void CharacterJump();

	void CharacterStopJump();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

