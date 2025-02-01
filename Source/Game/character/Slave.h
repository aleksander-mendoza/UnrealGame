// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../items/ItemObject.h"

/**
 *
 */
class Slave
{
public:
	virtual ~Slave() {}
	virtual void Punish() = 0;
	virtual bool AllowAttacking() = 0;
};
