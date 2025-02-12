// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthEvents.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHealthEvents : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class GAME_API IHealthEvents
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnFullHealth() = 0;
	virtual void OnFullMagic() = 0;
	virtual void OnFullStamina() = 0;
	virtual void OnHealthChanged(float health, float maxHealth) = 0;
	virtual void OnMagicChanged(float magic, float maxMagic) = 0;
	virtual void OnStaminaChanged(float stamina, float maxStamina) = 0;
	virtual void OnDamage(class AGameCharacter* attacker, float health) = 0;
	virtual void OnKilled(class AGameCharacter* killer) = 0;
};
