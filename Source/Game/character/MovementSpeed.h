// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "MovementSpeed.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class EMovementSpeed : uint8
{
	RUN        UMETA(DisplayName = "Run"),
	WALK        UMETA(DisplayName = "Walk"),
	SLOW_WALK  UMETA(DisplayName = "Slow walk")

};
