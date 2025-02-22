// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInstance.h"
#include "inv/CharacterInventory.h"
#include "../ui/inventory/Inventory.h"

void UItemInstance::refreshInventoryEntryWidget(){
	if (Owner!=nullptr){ 
		Owner->refreshInventoryEntryWidget(this);
	}
}
bool UItemInstance::use(bool leftHand)
{
	UCharacterInventory* characterInv = Cast<UCharacterInventory>(Owner);
	if (characterInv != nullptr) {
		return use(characterInv, leftHand);
	}
    return false;
}

TObjectPtr<UItemInstance> UItemInstance::drop(int count)
{
	return Owner==nullptr?nullptr:Owner->dropItem(this, count);
}
