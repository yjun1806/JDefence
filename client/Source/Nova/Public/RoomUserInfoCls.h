// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include <string>

/**
 * 
 */
class NOVA_API RoomUserInfoCls
{
public:
	RoomUserInfoCls();
	~RoomUserInfoCls();

public:
	std::string userid;
	int32 useridCode;
	bool isready;
	int32 MaxPoint;
};
