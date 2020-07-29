// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Blueprint/UserWidget.h"
#include "PlayMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class NOVA_API UPlayMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnReturnToLobbyClicked();


protected:
	UPROPERTY()
	class UButton* ResumeButton;

	UPROPERTY()
	class UButton* ReturnToLobbyButton;




	
};
