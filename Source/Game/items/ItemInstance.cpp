// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInstance.h"
#include "inv/CharacterInventory.h"
#include "../ui/inventory/Inventory.h"

bool UItemInstance::use(bool leftHand)
{
	UCharacterInventory* characterInv = Cast<UCharacterInventory>(Owner);
	if (characterInv != nullptr) {
		if (use(characterInv, leftHand)) {
			if (characterInv->InventoryWidget != nullptr) {
				characterInv->InventoryWidget->refresh();
			}
			return true;
		}
	}
    return false;
}

TObjectPtr<UItemInstance> UItemInstance::drop(int count)
{
	return Owner==nullptr?nullptr:Owner->dropItem(this, count);
}
