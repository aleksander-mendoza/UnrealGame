// Fill out your copyright notice in the Description page of Project Settings.


#include "Health.h"
#include "../GameCharacter.h"

// Sets default values for this component's properties
FHealth::FHealth()
{
	
}

void FHealth::setHealth(AGameCharacter* character, float hp){
	if (hp > MaxHealth) hp = MaxHealth;
	if (Widget)Widget->Stamina->SetPercent(hp / MaxHealth);
	character->OnHealthChanged(hp, MaxHealth);
	Health = hp;
}

bool FHealth::damage(AGameCharacter* character, AGameCharacter* attacker, float health) {
	check(health >= 0);
	if (Health >= health) {
		setHealth(character, Health - health);
		character->OnDamage(attacker, Health);
		return true;
	}
	else {
		character->OnKilled(attacker);
		return false;
	}


}

void FHealth::kill(AGameCharacter* character, AGameCharacter* killer) {
	Health = 0;
	character->OnKilled(killer);
}

void FHealth::heal(AGameCharacter* character, float health) {
	check(health >= 0);
	if (Health < MaxHealth) {
		setHealth(character, Health + health);
		if (Health >= MaxHealth) {
			character->OnFullHealth();
		}
	}
}

void FHealth::tick(AGameCharacter* character, float DeltaTime) {
	if (Magic < MaxMagic) {
		if (PreventMagicRegen > 0) {
			PreventMagicRegen -= DeltaTime;
		}
		else {
			setMagic(Magic + MagicRegenSpeed * DeltaTime);
		}
	}

	if (Health < MaxHealth) {
		if (PreventHealthRegen > 0) {
			PreventHealthRegen -= DeltaTime;
		}
		else {
			heal(character, HealthRegenSpeed * DeltaTime);
		}
	}

	if (Stamina < MaxStamina) {
		if (PreventStaminaRegen > 0) {
			PreventStaminaRegen -= DeltaTime;
		}
		else {
			setStamina(Stamina + StaminaRegenSpeed * DeltaTime);
		}
	}
}




