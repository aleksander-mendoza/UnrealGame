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
	UNARMED UMETA(DisplayName = "Unarmed"),
	BOW UMETA(DisplayName = "Holds bow"),
	BOW_AIMED UMETA(DisplayName = "Aims bow"),
	BARE_HANDED UMETA(DisplayName = "Bare-handed"),
	SINGLE_HANDED UMETA(DisplayName = "Single-handed"),
	DOUBLE_HANDED UMETA(DisplayName = "Double-handed"),
};

inline EMeleeAttackClass poseToAttack(EArmedPoseType pose) {
	switch (pose) {
	case EArmedPoseType::DOUBLE_HANDED:
		return EMeleeAttackClass::DOUBLE_HANDED_WEAPON;
	case EArmedPoseType::BARE_HANDED:
		return EMeleeAttackClass::BARE_HANDED;
	case EArmedPoseType::SINGLE_HANDED:
		return EMeleeAttackClass::SINGLE_HANDED_WEAPON;
	default:
		return EMeleeAttackClass::NONE;
	}
}