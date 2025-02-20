// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventory.h"
#include "../item/WeaponItem.h"
#include "../item/ClothingItem.h"
#include "../ItemInstance.h"


void UCharacterInventory::removeClothingItem(const UClothingItem* type, TObjectPtr<UItemInstance> item)
{
	check(item->ItemType == type);
	check(0 <= item->EquippedAt);
	check(item->EquippedAt < Clothes.Num());
	check(Clothes[item->EquippedAt] == item);
	check(Items.Contains(item));
	if (0 <= item->EquippedAt) {
		const int idx = item->EquippedAt;
		check(Clothes.Last()->EquippedAt >= 0);
		Clothes.Last()->EquippedAt = idx;
		Clothes.RemoveAtSwap(idx);
		check(idx >= Clothes.Num() || Clothes[idx]->EquippedAt == idx);
		item->EquippedAt = EQUIPPED_AT_NONE;
		check((occupiedClothingSlots | occupiedDeviousClothingSlots) == occupiedClothingSlots);
		occupiedClothingSlots &= ~type->ClothingSlots;
		if(type->isDevious()) occupiedDeviousClothingSlots &= ~type->ClothingSlots;
		check(isAllValid());
	}
}

void UCharacterInventory::addClothingItem(const UClothingItem* type, TObjectPtr<UItemInstance> item){
	check(Items.Contains(item));
	check(item->EquippedAt == EQUIPPED_AT_NONE);
	check((type->ClothingSlots & occupiedClothingSlots) == 0);
	check((occupiedClothingSlots | occupiedDeviousClothingSlots) == occupiedClothingSlots);
	item->EquippedAt = Clothes.Add(item);
	occupiedClothingSlots |= type->ClothingSlots;
	if (type->isDevious()) occupiedDeviousClothingSlots &= ~type->ClothingSlots;
	check(isAllValid());
	
}

bool UCharacterInventory::equipClothes(const UClothingItem* type, TObjectPtr<UItemInstance> owner)
{
	check(owner->ItemType == type);
	check(Items.Contains(owner));
	if ((occupiedDeviousClothingSlots & type->ClothingSlots) != 0) return false;
	if ((occupiedClothingSlots & type->ClothingSlots) != 0) {
		for (int clothesIdx = Clothes.Num(); clothesIdx >= 0; clothesIdx--) {
			TObjectPtr<UItemInstance>  equippedItemObj = Clothes[clothesIdx];
			const UClothingItem* equippedType = equippedItemObj->ItemType->asClothingItem();
			check(equippedType != nullptr);
			if ((equippedType->ClothingSlots & type->ClothingSlots) != 0) {
				check(!equippedType->isDevious());
				removeClothingItem(equippedType, equippedItemObj);
			}
		}
	}
	addClothingItem(type, owner);
	return true;
}

bool UCharacterInventory::unequipClothes(const UClothingItem* type, TObjectPtr<UItemInstance> owner)
{
	if (type->isDevious())return false;
	removeClothingItem(type, owner);
	return true;
}

bool UCharacterInventory::unequipProjectile()
{
	check(SelectedProjectile !=nullptr);
	check(SelectedProjectile->EquippedAt == EQUIPPED_AT_PROJECTILE);
	SelectedProjectile->EquippedAt = EQUIPPED_AT_NONE;
	SelectedProjectile = nullptr;
	return true;
}

bool UCharacterInventory::unequipDoubleHanded()
{
	check(LeftHand == RightHand);
	LeftHand->EquippedAt = EQUIPPED_AT_NONE;
	RightHand = nullptr;
	LeftHand = nullptr;
	check(isAllValid());
	return true;
}

bool UCharacterInventory::unequipLeftHand()
{
	check(LeftHand != RightHand);
	LeftHand->EquippedAt = EQUIPPED_AT_NONE;
	LeftHand = nullptr;
	check(isAllValid());
	return true;
}

bool UCharacterInventory::unequipRightHand()
{
	check(LeftHand != RightHand);
	RightHand->EquippedAt = EQUIPPED_AT_NONE;
	RightHand = nullptr;
	check(isAllValid());
	return true;
}

bool UCharacterInventory::equipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance> owner)
{
	check(owner->ItemType == type);
	check(owner->EquippedAt == EQUIPPED_AT_NONE);
	if (SelectedProjectile != nullptr) {
		unequipProjectile();
	}
	SelectedProjectile = owner;
	SelectedProjectile->EquippedAt = EQUIPPED_AT_PROJECTILE;
	check(isAllValid());
	return true;
}

bool UCharacterInventory::equipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	check(owner->ItemType == type);
	check(owner->EquippedAt == EQUIPPED_AT_NONE);
	if (LeftHand != nullptr) {
		if (LeftHand == RightHand) {
			unequipBothHands();
		}
		else {
			unequipLeftHand();
		}
	}
	if (RightHand != nullptr) {
		unequipRightHand();
	}
	LeftHand = owner;
	RightHand = owner;
	owner->EquippedAt = EQUIPPED_AT_DOUBLEHANDED;
	check(isAllValid());
	return true;
}

bool UCharacterInventory::equipSingleHanded(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner, bool leftHand)
{
	return leftHand ? equipLeftHand(type, owner) : equipRightHand(type, owner);
}

bool UCharacterInventory::equipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	check(owner->ItemType == (const UItem *) type);
	check(owner->EquippedAt == EQUIPPED_AT_NONE);
	check(isAllValid());
	if (LeftHand != nullptr) {
		unequipLeftHand();
	}
	LeftHand = owner;
	LeftHand->EquippedAt = EQUIPPED_AT_LEFT_HAND;
	check(isAllValid());
	return true;
}

bool UCharacterInventory::equipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	check(owner->ItemType == (const UItem*)type);
	check(owner->EquippedAt == EQUIPPED_AT_NONE);
	check(isAllValid());
	if (RightHand != nullptr) {
		unequipRightHand();
	}
	RightHand = owner;
	RightHand->EquippedAt = EQUIPPED_AT_RIGHT_HAND;
	check(isAllValid());
	return true;
}
