// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "GameFramework/GameModeBase.h"
#include "NovaGameInstance.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()


		ALobbyGameMode();

	virtual void BeginPlay() override;

public:
	

	UFUNCTION(BlueprintCallable, Category = "LogOut")
	void GameLogOut();

	UPROPERTY()
	UNovaGameInstance* gameInstance;
};
