// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"
#include "../../items/inv/GameCharacterInventory.h"
#include "../ItemInstance.h"




bool UOneHandedWeaponItem::equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const
{
	return user->equipSingleHanded(this, owner, leftHand);
	
}

bool UOneHandedWeaponItem::unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const
{
	check(owner->ItemType == this);
	if (owner->EquippedAt==EQUIPPED_AT_LEFT_HAND) {
		check(user->LeftHand == owner);
		return user->unequipLeftHand();
	}
	else {
		check(owner->EquippedAt == EQUIPPED_AT_RIGHT_HAND || owner->EquippedAt == EQUIPPED_AT_NONE);
		check(user->RightHand == owner || owner->EquippedAt == EQUIPPED_AT_NONE);
		return user->unequipRightHand();
	}
}

float UOneHandedWeaponItem::getTotalDamage(UInventoryAndHealth* user) const
{
	return Damage * user->Health.SingleHandedWeaponDamageMultiplier;
}

bool UProjectileItem::equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const
{
	user->equipProjectile(this, owner);
	return true;
	
}

bool UProjectileItem::unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const
{
	check(owner->ItemType == this);
	check(owner->EquippedAt == EQUIPPED_AT_NONE || (user->SelectedProjectile == owner && owner->EquippedAt == EQUIPPED_AT_PROJECTILE));
	user->unequipProjectile();
	return true;
}

bool UDoubleHandedWeaponItem::equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const
{
	return user->equipDoubleHanded(this, owner);
}

bool UDoubleHandedWeaponItem::unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const
{
	check(owner->ItemType == this);
	if (owner->EquippedAt == EQUIPPED_AT_DOUBLEHANDED) {
		return user->onUnequipDoubleHanded();
	}
	return true;
}

float UDoubleHandedWeaponItem::getTotalDamage(UInventoryAndHealth* user) const
{
	return Damage * user->Health.DoubleHandedWeaponDamageMultiplier;
}

bool UWeaponItem::startAttack(UCombatInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const
{
	return Moveset->startAttack(user, leftHand);
}

void UWeaponItem::endAttack(UCombatInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const
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
