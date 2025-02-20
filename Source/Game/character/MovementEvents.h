// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class MovementEvents
{
public:
	virtual ~MovementEvents() {}
	virtual void OnBowReleased() = 0;
	virtual bool AllowAttacking() = 0;
};
