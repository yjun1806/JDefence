// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "NovaGameInstance.h"
#include "LobbyWidget.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

/**
 * 
 */
class NOVA_API NovaSocketListener : public FRunnable
{
public:
	FRunnableThread* Thread;

	static NovaSocketListener* Runnable;

	static bool bThreadRun;
public:
	NovaSocketListener();
	~NovaSocketListener();

	virtual bool   Init();
	virtual uint32 Run();


	//ULobbyWidget* lobbywidget;

	static NovaSocketListener* RunSocketListening();

	static void Shutdown();


};
