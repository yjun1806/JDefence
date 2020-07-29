// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "GameFramework/GameModeBase.h"
#include "RoomGameMode.generated.h"


/**
 * 
 */
UCLASS()
class NOVA_API ARoomGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ARoomGameMode();

	virtual void BeginPlay() override;
	
public:
	
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	URoomWidgetHUD* RoomWidgetClass;

	UPROPERTY()
	class URoomWidgetHUD* RoomWidgetHUD;

	void UpdateRoomUserList();*/
	
};
