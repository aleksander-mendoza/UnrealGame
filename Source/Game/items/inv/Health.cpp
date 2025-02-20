// Fill out your copyright notice in the Description page of Project Settings.


#include "Health.h"
#include "../../GameCharacter.h"
#include "../../ui/NpcHealthBar.h"

void FHealth::Tick(float DeltaTime)
{
	PreventHealthRegen = FMath::Max(PreventHealthRegen - DeltaTime, 0.);
	PreventStaminaRegen = FMath::Max(PreventStaminaRegen - DeltaTime, 0.);
	PreventMagicRegen = FMath::Max(PreventMagicRegen - DeltaTime, 0.);

	Health = FMath::Clamp(Health +  (PreventHealthRegen > 0 ? 0 : HealthRegenSpeed * DeltaTime), 0., MaxHealth);
	Stamina = FMath::Clamp(Stamina +  (PreventStaminaRegen > 0 ? 0 : StaminaRegenSpeed * DeltaTime), 0., MaxStamina);
	Magic = FMath::Clamp(Magic, + (PreventMagicRegen > 0 ? 0 : MagicRegenSpeed * DeltaTime), MaxMagic);
	Arousal = FMath::Clamp(Arousal +  (ArousalRegenSpeed * DeltaTime), 0., 100.);
	

}
