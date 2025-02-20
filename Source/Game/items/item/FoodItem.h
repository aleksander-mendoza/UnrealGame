// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "FoodItem.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UFoodItem : public UItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RestoresHealth = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RestoresWarmth = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RestoresStamina = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RestoresMagic = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RestoresArousal = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegeneratesHealthDuration = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegeneratesHealthSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegeneratesStaminaDuration = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegeneratesStaminaSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegeneratesMagicDuration = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegeneratesMagicSpeed = 0;

	
	virtual bool use(UCharacterInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const override {
		return affect(nullptr, user, owner);
	}

	
	virtual bool affect(UCharacterInventory* user, UCharacterInventory* affected, TObjectPtr<UItemInstance> owner) const;

};