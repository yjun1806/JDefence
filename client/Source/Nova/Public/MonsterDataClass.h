// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include <string>

/**
 * 
 */
class NOVA_API MonsterDataClass
{
public:
	MonsterDataClass();
	~MonsterDataClass();

public:
	int32 player_name = 0;
	int32 health = 1000;
	bool IsAlive = true;
	bool IsAttacking = false;
	int32 AttackCountNumber = 0;
	int32 MonsterType = 0;
	

	float Loc_X = 0.0f;
	float Loc_Y = 0.0f;
	float Loc_Z = 0.0f;

};
