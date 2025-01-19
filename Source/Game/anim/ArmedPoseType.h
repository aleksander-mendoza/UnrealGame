// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
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
	BARE_HANDED UMETA(DisplayName = "Holds bow"),
	SINGLE_HANDED UMETA(DisplayName = "Single-handed"),
	DOUBLE_HANDED UMETA(DisplayName = "Double-handed"),
};