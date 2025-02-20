// Fill out your copyright notice in the Description page of Project Settings.


#include "FoodItem.h"
#include "../../items/inv/GameCharacterInventory.h"


bool UFoodItem::affect(UCharacterInventory* user, UCharacterInventory* affected, TObjectPtr<UItemInstance> owner) const
{
	return false;
	
}
