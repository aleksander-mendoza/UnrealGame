// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../items/ItemObject.h"

/**
 *
 */
class SlaveOwner
{
public:
	virtual ~SlaveOwner() {}
	virtual void OnBowReleased() = 0;
	virtual bool AllowAttacking() = 0;
};
