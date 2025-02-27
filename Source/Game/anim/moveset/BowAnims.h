// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAnims.h"
#include "BowAnims.generated.h"


/**
 * 
 */
UCLASS(BlueprintType)
class GAME_API UBowAnims : public UWeaponAnims
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> BowDrawArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> BowShootArrow;

	virtual bool startAttack(UCombatInventory* user, bool leftHand) const override;
	virtual void endAttack(UCombatInventory* user, bool leftHand) const override;
	virtual void cancelAttack(UCombatInventory* user) const override;
	
};
