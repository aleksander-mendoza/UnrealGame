// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResponseAction.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class EResponseAction : uint8
{
	CONTINUE UMETA(DisplayName = "Continue conversation"),
	END UMETA(DisplayName = "End conversation"),
	TRADE UMETA(DisplayName = "Begin trade"),
	EXCHANGE UMETA(DisplayName = "Begin exchange"),
	GIVE UMETA(DisplayName = "Player gets item"),
	TELEPORT UMETA(DisplayName = "Player teleports"),
	TURN_HOSTILE UMETA(DisplayName = "NPC turns hostile"),
	BECOME_FOLLOWER UMETA(DisplayName = "NPC becomes follower"),
	START_SEX UMETA(DisplayName = "Initiates sex"),
	START_QUEST UMETA(DisplayName = "Starts quest"),
};