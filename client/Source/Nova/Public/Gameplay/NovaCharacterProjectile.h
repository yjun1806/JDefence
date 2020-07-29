// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "GameFramework/Actor.h"
#include "NovaCharacterProjectile.generated.h"

UCLASS()
class NOVA_API ANovaCharacterProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANovaCharacterProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void Skill_2_HitCheck(const FHitResult& result);
	
	UFUNCTION(BlueprintCallable)
		void WhoMakeThisProjectile(const int32 name);

	UPROPERTY()
		int32 Name;
	
};
