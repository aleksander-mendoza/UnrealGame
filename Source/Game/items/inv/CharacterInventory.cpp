// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventory.h"
#include "../item/WeaponItem.h"
#include "../item/ClothingItem.h"
#include "../ItemInstance.h"


TObjectPtr<UItemInstance> UCharacterInventory::dropItem(TObjectPtr<UItemInstance> item, int quantity)
{
	item->unequip(this);
	return Super::dropItem(item, quantity);
}

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
		refreshInventoryEntryWidget(item);
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
	refreshInventoryEntryWidget(item);
	
}

bool UCharacterInventory::equipClothes(const UClothingItem* type, TObjectPtr<UItemInstance> owner)
{
	if (owner->EquippedAt == EQUIPPED_AT_NONE) {
		check(owner->ItemType == type);
		check(owner->Owner == this);
		check(Items.Contains(owner));
		if ((occupiedDeviousClothingSlots & type->ClothingSlots) != 0) return false;
		if (type->isSkeletalMeshNull())return false;
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
	}
	return true;
}

bool UCharacterInventory::unequipClothes(const UClothingItem* type, TObjectPtr<UItemInstance> owner)
{
	if (owner->EquippedAt >= 0) {
		check(owner->Owner == this);
		if (type->isDevious())return false;
		removeClothingItem(type, owner);
	}
	return true;
}

bool UCharacterInventory::onUnequipProjectile()
{
	check(SelectedProjectile !=nullptr);
	check(SelectedProjectile->EquippedAt == EQUIPPED_AT_PROJECTILE);
	SelectedProjectile->EquippedAt = EQUIPPED_AT_NONE;
	refreshInventoryEntryWidget(SelectedProjectile);
	SelectedProjectile = nullptr;
	return true;
}

bool UCharacterInventory::onUnequipDoubleHanded()
{
	check(LeftHand == RightHand);
	check(LeftHand->EquippedAt == EQUIPPED_AT_DOUBLEHANDED);
	LeftHand->EquippedAt = EQUIPPED_AT_NONE;
	refreshInventoryEntryWidget(LeftHand);
	RightHand = nullptr;
	LeftHand = nullptr;
	check(isAllValid());
	return true;
}

bool UCharacterInventory::onUnequipLeftHand()
{
	check(LeftHand != RightHand);
	LeftHand->EquippedAt = EQUIPPED_AT_NONE;
	refreshInventoryEntryWidget(LeftHand);
	LeftHand = nullptr;
	check(isAllValid());
	return true;
}
bool UCharacterInventory::onUnequipRightHand()
{
	check(LeftHand != RightHand);
	RightHand->EquippedAt = EQUIPPED_AT_NONE;
	refreshInventoryEntryWidget(RightHand);
	RightHand = nullptr;
	check(isAllValid());
	return true;
}

bool UCharacterInventory::onEquipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance> owner)
{
	check(owner->ItemType == type);
	check(owner->EquippedAt == EQUIPPED_AT_NONE);
	check(SelectedProjectile == nullptr);
	SelectedProjectile = owner;
	SelectedProjectile->EquippedAt = EQUIPPED_AT_PROJECTILE;
	refreshInventoryEntryWidget(SelectedProjectile);
	check(isAllValid());
	return true;
}

bool UCharacterInventory::onEquipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	check(owner->ItemType == type);
	check(owner->EquippedAt == EQUIPPED_AT_NONE);
	check(LeftHand == nullptr);
	check(RightHand == nullptr);
	LeftHand = owner;
	RightHand = owner;
	owner->EquippedAt = EQUIPPED_AT_DOUBLEHANDED;
	refreshInventoryEntryWidget(owner);
	check(isAllValid());
	return true;
}

bool UCharacterInventory::onEquipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	check(owner->ItemType == (const UItem *) type);
	check(owner->EquippedAt == EQUIPPED_AT_NONE);
	check(isAllValid());
	check(LeftHand == nullptr);
	LeftHand = owner;
	LeftHand->EquippedAt = EQUIPPED_AT_LEFT_HAND;
	check(isAllValid());
	refreshInventoryEntryWidget(owner);
	return true;
}

bool UCharacterInventory::onEquipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	check(owner->ItemType == (const UItem*)type);
	check(owner->EquippedAt == EQUIPPED_AT_NONE);
	check(isAllValid());
	check(RightHand == nullptr);
	RightHand = owner;
	RightHand->EquippedAt = EQUIPPED_AT_RIGHT_HAND;
	check(isAllValid());
	refreshInventoryEntryWidget(owner);
	return true;
}

void UCharacterInventory::stripClothes()
{
	for (int i = 0; i < Clothes.Num(); i++) {
		Clothes[i]->EquippedAt = EQUIPPED_AT_NONE;
	}
	Clothes.Empty();
	occupiedClothingSlots = 0;
	occupiedDeviousClothingSlots = 0;
}

void UCharacterInventory::stripHands()
{
	if (LeftHand != nullptr) {
		LeftHand->EquippedAt = EQUIPPED_AT_NONE;
		LeftHand = nullptr;
	}
	if (RightHand != nullptr) {
		RightHand->EquippedAt = EQUIPPED_AT_NONE;
		RightHand = nullptr;
	}
}

void UCharacterInventory::clearInventory()
{
	stripAll();
	Super::clearInventory();
}
