// Fill out your copyright notice in the Description page of Project Settings.


#include "Health.h"
#include "../blender/utildefines.h"

// Sets default values for this component's properties
UHealth::UHealth()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1;
	
	// ...
}


// Called when the game starts
void UHealth::BeginPlay()
{
	//Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Magic < MaxMagic) {
		if (PreventMagicRegen > 0) {
			PreventMagicRegen -= DeltaTime;
		}
		else {
			setMagic(math::min(Magic + MagicRegenSpeed * DeltaTime, MaxMagic));
		}
	}
	else {

	}

	if (Health < MaxHealth) {
		if (PreventHealthRegen > 0) {
			PreventHealthRegen -= DeltaTime;
		}
		else {
			setHealth(math::min(Health + HealthRegenSpeed * DeltaTime, MaxHealth));
		}
	}
	else {

	}

	if (Stamina < MaxStamina) {
		if (PreventStaminaRegen > 0) {
			PreventStaminaRegen -= DeltaTime;
		}
		else {
			setStamina(math::min(Stamina + StaminaRegenSpeed * DeltaTime, MaxStamina));
		}
	}
	else {

	}
	
}

