// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "SheathSocketType.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class ESheathSocketType : uint8
{
	UNSHEATHED     UMETA(DisplayName = "Unsheathed"),
	SHEATHED_SIDE  UMETA(DisplayName = "Sheathed on the side"),
	SHEATHED_BACK  UMETA(DisplayName = "Sheathed on the back")

};
