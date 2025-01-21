// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "AttackHandedness.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class EAttackHandedness : uint8
{
	NONE = 0 UMETA(DisplayName = "No attack"),
	LEFT_HANDED = 1          UMETA(DisplayName = "Left-handed attack"),
	RIGHT_HANDED = 2  UMETA(DisplayName = "Right-handed attack"),
	BOTH_HANDED = 3 UMETA(DisplayName = "Both-handed attack")

};
inline bool isLeftHanded(EAttackHandedness handedness) {
	return (uint8(handedness) & uint8(EAttackHandedness::LEFT_HANDED)) != 0;
}
inline bool isRightHanded(EAttackHandedness handedness) {
	return (uint8(handedness) & uint8(EAttackHandedness::RIGHT_HANDED)) != 0;
}