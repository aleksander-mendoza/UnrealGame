// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ItemSlot.h"
#include "ClothingItem.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UClothingItem : public UItem
{
	GENERATED_BODY()
public:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Armor = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Warmth = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ClothingSlots = ItemSlot::NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 EnchantmentSlots = 0;


	/**Devious devices cannot be uneqipped without a key/lockpick/struggle. The higher the level the more difficult it is to escape.
	Level 0 means the item is not devious (can be freely unequipped). Level 1,2,3... requires keys of the corresponding level.
	Level -1, -2, -3 cannot be picked up or struggled out of and must be opened with corresponding key 1,2,3...*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int DeviousLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsFemale = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsHeavyArmor = false;

	virtual bool equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const override;

	virtual bool unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const override;

	virtual int32 getClothingSlots() const override {
		return ClothingSlots;
	}
	virtual int getDeviousLevel() const override{
		return DeviousLevel;
	}
	virtual float getWarmth() const override{
		return Warmth;
	}
	virtual const UClothingItem* asClothingItem() const override {
		return this;
	}
	
	inline bool isDevious() const {
		return DeviousLevel != 0;
	}
};
