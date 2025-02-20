// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorInventory.h"

TObjectPtr<UItemInstance> UActorInventory::removeItem(TObjectPtr<UItemInstance> item, int quantity)
{
	check(item->EquippedAt == EQUIPPED_AT_NONE);
	TObjectPtr<UItemInstance> popped = item->popCount(GetWorld(), quantity);
	if (popped == item) {
		bool b = Items.RemoveSingleSwap(popped) != 0;
		check(b);
	}
	return popped;
}

TObjectPtr<UItemInstance> UActorInventory::spawnItem(UItem* itemType, int quantity)
{
	TObjectPtr<UItemInstance> item = NewObject<UItemInstance>(GetWorld());
	item->ItemType = itemType;
	item->Quantity = quantity;
	item->Durbility = itemType->Durbility;
	addItem(item);
	return item;
}