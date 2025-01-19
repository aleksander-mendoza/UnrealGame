// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LocomotionType.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class ELocomotionType : uint8
{
	WALK UMETA(DisplayName = "Walking"),
	SLOW_WALK UMETA(DisplayName = "Slow walking"),
	CROUCH UMETA(DisplayName = "Crouched"),
	SWIM UMETA(DisplayName = "Swimming"),
	RUN UMETA(DisplayName = "Running"),
};