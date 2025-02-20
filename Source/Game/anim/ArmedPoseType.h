// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "../anim/MeleeAttackClass.h"
#include "ArmedPoseType.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EArmedPoseType : uint8
{
	UNARMED=0 UMETA(DisplayName = "Unarmed"),
	BARE_HANDED=1 UMETA(DisplayName = "Bare-handed"),
	SINGLE_HANDED=2 UMETA(DisplayName = "Single-handed"),
	MAGIC = 3 UMETA(DisplayName = "Single-handed"),
	BOW=4 UMETA(DisplayName = "Holds bow"),
	BOW_AIMED=5 UMETA(DisplayName = "Aims bow"),
	DOUBLE_HANDED=6 UMETA(DisplayName = "Double-handed"),
};