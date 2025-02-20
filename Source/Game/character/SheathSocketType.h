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
	BOW     UMETA(DisplayName = "Sheathed as a bow"),
	SIDE  UMETA(DisplayName = "Sheathed on the side"),
	BACK  UMETA(DisplayName = "Sheathed on the back")

};
