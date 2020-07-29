// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include <string>

/**
 * 
 */
class NOVA_API PlayerInfo
{
public:
	PlayerInfo();
	~PlayerInfo();

public:
	std::string Userid;
	int32 IDcode = 0;
	int32 NowGameStateCode;
	int32 MaxPoint = 0;
};
