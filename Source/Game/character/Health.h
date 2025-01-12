// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../ui/Status.h"
#include "../items/ItemObject.h"
#include "Health.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UHealth : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealth();


	float PreventStaminaRegen = 0;
	float PreventHealthRegen = 0;
	float PreventMagicRegen = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Stamina = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxStamina=100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StaminaRegenSpeed=20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HealthRegenSpeed=5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Warmth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxWarmth = 100;

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Magic = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxMagic = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MagicRegenSpeed=20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dirtiness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Arousal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Defence = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CarriedWeight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxCarriedWeight = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LightArmorMultiplier = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeavyArmorMultiplier = 1;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BareHandDamage = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SingleHandedWeaponDamageMultiplier = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DoubleHandedWeaponDamageMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StealthDamageMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BowDamageMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpearDamageMultiplier = 1;

	TObjectPtr<UStatus> Widget=nullptr;

	float getDamage(UItemObject* item) {
		if (item == nullptr) {
			return BareHandDamage;
		}
		FItem * row = item->getRow();
		switch(row->Class) {
		case EItemClass::DOUBLE_HANDED_WEAPON:
			return row->Damage * DoubleHandedWeaponDamageMultiplier;
		case EItemClass::SINGLE_HANDED_QUIET_WEAPON:
			return row->Damage * StealthDamageMultiplier;
		case EItemClass::SINGLE_HANDED_WEAPON:
			return row->Damage * SingleHandedWeaponDamageMultiplier;
		case EItemClass::BOW:
			return row->Damage * BowDamageMultiplier;
		case EItemClass::SPEAR:
			return row->Damage * SpearDamageMultiplier;
		default:
			return row->Damage + BareHandDamage;
		}
	}
	void setWidget(TObjectPtr<UStatus> widget) {
		Widget = widget;
		Widget->healthComponenet = this;
		resetWidget();
	}

	void resetWidget() {
		if (IsValid(Widget)) {
			if(Widget->Stamina)Widget->Stamina->SetPercent(Stamina / MaxStamina);
			if(Widget->Health)Widget->Health->SetPercent(Health / MaxHealth);
			if(Widget->Magic)Widget->Magic->SetPercent(Magic / MaxMagic);
			if(Widget->Arousal)Widget->Arousal->SetPercent(Arousal);
		}
	}

	void setStamina(float stamina) {
		if (Widget)Widget->Stamina->SetPercent(stamina / MaxStamina);
		Stamina = stamina;
	}

	bool takeStamina(float stamina) {
		if (Stamina >= stamina) {
			setStamina(Stamina - stamina);
			return true;
		}
		return false;
	}

	void setHealth(float hp) {
		if (Widget)Widget->Stamina->SetPercent(hp / MaxHealth);
		Health = hp;
	}

	bool takeHealth(float health) {
		if (Health >= health) {
			setHealth(Health - health);
			return true;
		}
		return false;
		
	}

	void setMagic(float m) {
		if (Widget)Widget->Stamina->SetPercent(m / MaxMagic);
		Magic = m;
	}

	bool takeMagic(float m) {
		if (Magic >= m) {
			setMagic(Magic - m);
			return true;
		}
		return false;

	}
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
