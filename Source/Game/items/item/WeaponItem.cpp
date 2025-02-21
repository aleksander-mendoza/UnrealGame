// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"
#include "../../items/inv/GameCharacterInventory.h"
#include "../ItemInstance.h"




bool UOneHandedWeaponItem::equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const
{
	return user->onEquipSingleHanded(this, owner, leftHand);
	
}

bool UOneHandedWeaponItem::unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const
{
	check(owner->ItemType == this);
	if (owner->EquippedAt==EQUIPPED_AT_LEFT_HAND) {
		check(user->LeftHand == owner);
		return user->onUnequipLeftHand();
	}
	else {
		check(owner->EquippedAt == EQUIPPED_AT_RIGHT_HAND);
		check(user->RightHand == owner);
		return user->onUnequipRightHand();
	}
}

float UOneHandedWeaponItem::getTotalDamage(UInventoryAndHealth* user) const
{
	return Damage * user->Health.SingleHandedWeaponDamageMultiplier;
}

bool UProjectileItem::equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const
{
	return user->onEquipProjectile(this, owner);
	
}

bool UProjectileItem::unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const
{
	check(owner->ItemType == this);
	check(owner->EquippedAt == EQUIPPED_AT_PROJECTILE);
	check(user->SelectedProjectile == owner);
	return user->onUnequipProjectile();
}

bool UDoubleHandedWeaponItem::equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const
{
	return user->onEquipDoubleHanded(this, owner);
}

bool UDoubleHandedWeaponItem::unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const
{
	check(owner->ItemType == this);
	check(owner->EquippedAt == EQUIPPED_AT_DOUBLEHANDED);
	check(user->LeftHand == owner);
	check(user->RightHand == owner);
	return user->onUnequipDoubleHanded();
}

float UDoubleHandedWeaponItem::getTotalDamage(UInventoryAndHealth* user) const
{
	return Damage * user->Health.DoubleHandedWeaponDamageMultiplier;
}

bool UWeaponItem::startAttack(UGameCharacterInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const
{
	return Moveset->startAttack(user, leftHand);
}

void UWeaponItem::endAttack(UGameCharacterInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const
{
	Moveset->endAttack(user, leftHand);
}


float UStealthWeaponItem::getTotalDamage(UInventoryAndHealth* user) const
{
	return Damage * user->Health.StealthDamageMultiplier;
}

float UBowItem::getTotalDamage(UInventoryAndHealth* user) const
{
	return Damage * user->Health.BowDamageMultiplier;
}

float USpearItem::getTotalDamage(UInventoryAndHealth* user) const
{
	return Damage * user->Health.SpearDamageMultiplier;
}
