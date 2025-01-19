// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "MeleeAttackClass.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class EMeleeAttackClass : uint8
{
	NONE UMETA(DisplayName = "No attack"),
	BARE_HANDED           UMETA(DisplayName = "Bare handed attack"),
	SINGLE_HANDED_WEAPON  UMETA(DisplayName = "Single-handed weapon attack"),
	DOUBLE_HANDED_WEAPON  UMETA(DisplayName = "Double-handed weapon attack")

};