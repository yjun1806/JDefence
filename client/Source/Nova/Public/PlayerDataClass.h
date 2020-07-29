// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nova.h"
#include <string>

/**
 * 
 */
class NOVA_API PlayerDataClass
{
public:
	PlayerDataClass();
	~PlayerDataClass();

public:
	std::string CharacterName = "";
	int32 player_name = 0;
	int32 health = 1000;
	bool IsAlive = true;
	bool IsAttacking = false;
	int32 AttackCountNumber = 0;
	bool IsSkillUsing = false;
	int32 SkillNumber = 0;
	int32 TotalReceivedDamage = 0;
	int32 TotalDied = 0;
	int32 TotalDamageToMonster = 0;
	int32 KilledMonster = 0;
	int32 KilledMonster_Rare = 0;
	int32 KilledMonster_Boss = 0;

	float Loc_X = 0.0f;
	float Loc_Y = 0.0f;
	float Loc_Z = 0.0f;

	float Rot_Pitch = 0.0f;
	float Rot_Roll = 0.0f;
	float Rot_Yaw = 0.0f;

	float Voc_VX = 0.0f;
	float Voc_VY = 0.0f;
	float Voc_VZ = 0.0f;
};
