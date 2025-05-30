// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "../../ui/inventory/InventoryEntry.h"
#include "../ItemInstance.h"
#include "../../GameCharacter.h"

bool UItem::use(UCharacterInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const
{
	if (owner->IsEquipped()) {
		return unequip(user, owner);
	}
	else {
		return equip(user, owner, leftHand);
	}
}

TObjectPtr<UItemInstance> UItem::drop(UCharacterInventory* user, TObjectPtr<UItemInstance> owner, int count) const
{
	if (owner->IsEquipped() && owner->Quantity <= count) {
		if (!unequip(user, owner))return nullptr;
	}
	TObjectPtr<UItemInstance> dropped = user->dropItem(owner, count);
	return dropped;
}

TObjectPtr<UItemInstance> UItem::spawn(UObject* outer, int count)
{
	TObjectPtr<UItemInstance> p = NewObject<UItemInstance>(outer, UItemInstance::StaticClass());
	p->ItemType = this;
	p->Quantity = count;
	p->Durability = Durability;
	return p;
}

void UItem::sample(UActorInventory* inv, int count)
{
	if (count > 0) {
		inv->addItem(spawn(inv->GetWorld(), count));
	}
}
