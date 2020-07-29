// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include <string>
#include "RoomUserInfoCls.h"

/**
 * 
 */
class NOVA_API RoomInfoCls
{
public:
	RoomInfoCls();
	~RoomInfoCls();

public:
	std::string RoomName;
	int32 RoomNumber;
	int32 MaxUserNumber;
	int32 CurrentUserNumber;
	std::string Password;
	int32 RoomOwner;
	bool IsThisRoomPlayingGame;
	TArray<class RoomUserInfoCls*> user_list;
	int32 FountainHealth;
	bool IsThisRoomLock;
	bool IsGameEnd = false;
	bool EndWidget = false;
	bool IsGameWin = false;

};
