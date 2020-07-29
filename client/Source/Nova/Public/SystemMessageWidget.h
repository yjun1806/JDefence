// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "SystemMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API USystemMessageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:


	UFUNCTION(BlueprintImplementableEvent)
		void SetSystemMessage(int32 Msgcode);

};
