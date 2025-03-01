// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonTileType.generated.h"


/**
 *
 */
UENUM(BlueprintType)
enum class EDungeonTileType : uint8
{
	FLOOR       UMETA(DisplayName = "Floor"),
	ONE_SIDE_WALL        UMETA(DisplayName = "One side wall"),
	TWO_SIDE_WALL        UMETA(DisplayName = "Two side wall"),
	DOOR		UMETA(DisplayName = "Door"),
	STAIRS		UMETA(DisplayName = "Stairs"),
	CEILING		UMETA(DisplayName = "Ceiling"),
	CONVEX_CORNER		UMETA(DisplayName = "Convex corner"),
	CONCAVE_CORNER		UMETA(DisplayName = "Concave corner"),
};