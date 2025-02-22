// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothingItem.h"
#include "../ItemInstance.h"
#include "../../items/inv/GameCharacterInventory.h"


bool UClothingItem::equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const
{
	return user->equipClothes(this, owner);
}


bool UClothingItem::unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const
{
	return user->unequipClothes(this, owner);
}
