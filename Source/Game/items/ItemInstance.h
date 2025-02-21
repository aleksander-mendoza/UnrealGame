// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "item/Item.h"
#include "ItemInstance.generated.h"

#define EQUIPPED_AT_PROJECTILE -5
#define EQUIPPED_AT_DOUBLEHANDED -4
#define EQUIPPED_AT_NONE -3
#define EQUIPPED_AT_LEFT_HAND -2
#define EQUIPPED_AT_RIGHT_HAND -1

class UInventoryEntry;
class UActorInventory;
/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UItemInstance : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItem * ItemType = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Durability = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Quantity = 1;

	// -5 = selected as projectile
	// -4 = held in both hands
	// -3 = not equipped
	// -2 = held in left hand
	// -1 = held in right hand
	// n>=0 = equipped as clothes at n-th index in clothes stack
	UPROPERTY(BlueprintReadOnly)
	int EquippedAt = EQUIPPED_AT_NONE;
	
	UPROPERTY()
	UActorInventory* Owner = nullptr;

	inline bool IsEquipped() const {
		return EquippedAt != EQUIPPED_AT_NONE;
	}
	inline float getDamage() const {
		return ItemType->getDamage();
	}
	inline bool isSkeletal() const {
		return ItemType->isSkeletal();
	}
	inline float getTotalDamage(UInventoryAndHealth* user) const{
		return ItemType->getTotalDamage(user);
	}
	inline USkeletalMesh* getSkeletalMesh() {
		return ItemType->getSkeletalMesh();
	}
	inline bool isLeftTheDominantHand() {
		return ItemType->isLeftTheDominantHand();
	}
	inline UStaticMesh* getMesh() {
		return ItemType->getMesh();
	}
	inline const UWeaponAnims * getMoveset() const {
		return ItemType->getMoveset();
	}
	inline bool startAttack(UGameCharacterInventory* user, bool leftHand) {
		return ItemType->startAttack(user, this, leftHand);
	}
	inline void endAttack(UGameCharacterInventory* user, bool leftHand) {
		return ItemType->endAttack(user, this, leftHand);
	}
	inline void cancelAttack(UGameCharacterInventory* user) {
		return ItemType->cancelAttack(user, this);
	}
	inline bool use(UCharacterInventory* user, bool leftHand) {
		return ItemType->use(user, this, leftHand);
	}
	bool use(bool leftHand);
	inline bool unequip(UCharacterInventory* user) {
		return ItemType->unequip(user, this);
	}
	inline bool equip(UCharacterInventory* user, bool leftHand) {
		return ItemType->equip(user, this, leftHand);
	}
	TObjectPtr<UItemInstance> drop(int count = 1);
	inline bool affect(UCharacterInventory* user, UCharacterInventory* affected)  {
		return ItemType->affect(user, affected, this);
	}

	
	inline TObjectPtr<UItemInstance> popCount(UObject * outer, int count=1) {
		if (Quantity > count) {
			Quantity -= count;
			return ItemType->spawn(outer, count);
		}
		else {
			EquippedAt = EQUIPPED_AT_NONE;
			return this;
		}
	}
};

