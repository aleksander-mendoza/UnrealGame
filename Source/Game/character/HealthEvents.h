// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class HealthEvents
{
public:
	virtual ~HealthEvents() {}
	virtual void OnFullHealth() = 0;
	virtual void OnFullMagic() = 0;
	virtual void OnFullStamina() = 0;
	virtual void OnHealthChanged(float health, float maxHealth) = 0;
	virtual void OnMagicChanged(float magic, float maxMagic) = 0;
	virtual void OnStaminaChanged(float stamina, float maxStamina) = 0;
	virtual void OnDamage(class AGameCharacter* attacker, float health) = 0;
	virtual void OnKilled(class AGameCharacter* killer) = 0;
};
