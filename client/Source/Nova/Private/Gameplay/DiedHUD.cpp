// Fill out your copyright notice in the Description page of Project Settings.

#include "DiedHUD.h"
#include "Components/Button.h"
#include "NovaGameInstance.h"
#include "NovaPlayerController.h"



void UDiedHUD::NativeConstruct()
{
	ImmediatelyRevivalBtn = Cast<UButton>(GetWidgetFromName(TEXT("ImmediatelyRevivalButton")));
	RevivalBtn = Cast<UButton>(GetWidgetFromName(TEXT("RevivalButton")));

	ImmediatelyRevivalBtn->OnClicked.AddDynamic(this, &UDiedHUD::CharacterRevivalClicked);
	RevivalBtn->OnClicked.AddDynamic(this, &UDiedHUD::CharacterRevivalFullHPClicked);
}

void UDiedHUD::CharacterRevivalClicked()
{
	UNovaGameInstance::NovaSocketcli->SendRevival(false);
	if (UNovaGameInstance::MyCharacterData->IsPlayerAlive == true) {
		UNovaGameInstance::MyCharacterData->CharacterRevival();

		RemoveFromViewport();

		auto Controller = Cast<ANovaPlayerController>(UNovaGameInstance::MyCharacterData->GetController());
		FInputModeGameAndUI GameMode;
		Controller->SetInputMode(GameMode);
		Controller->bShowMouseCursor = true;
	}
}

void UDiedHUD::CharacterRevivalFullHPClicked()
{
	UNovaGameInstance::NovaSocketcli->SendRevival(true);

	if (UNovaGameInstance::MyCharacterData->IsPlayerAlive == true) {
		UNovaGameInstance::MyCharacterData->CharacterRevival();
		RemoveFromViewport();

		auto Controller = Cast<ANovaPlayerController>(UNovaGameInstance::MyCharacterData->GetController());
		FInputModeGameAndUI GameMode;
		Controller->SetInputMode(GameMode);
		Controller->bShowMouseCursor = true;
	}
}

