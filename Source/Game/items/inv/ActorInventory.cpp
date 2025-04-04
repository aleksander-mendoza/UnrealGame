// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorInventory.h"
#include "../loot/Loot.h"
#include "../../ui/inventory/Inventory.h"

void UActorInventory::InitializeComponent()
{
	Super::InitializeComponent();
	if (IsValid(Loot)) {
		resetInventory();
	}
} 
void  UActorInventory::addItem(TObjectPtr<UItemInstance> item)
{
	check(item->EquippedAt == EQUIPPED_AT_NONE);
	check(item->Owner == nullptr);
	item->Owner = this;
	Items.Add(item);
	addInventoryEntryWidget(item);
	
}

TObjectPtr<UItemInstance> UActorInventory::removeItem(TObjectPtr<UItemInstance> item, int quantity)
{
	check(item->EquippedAt == EQUIPPED_AT_NONE);
	check(item->Owner == this);
	TObjectPtr<UItemInstance> popped = item->popCount(GetWorld(), quantity);
	if (popped == item) {
		popped->Owner = nullptr;
		bool b = Items.RemoveSingleSwap(popped) != 0;
		check(b);
		removeInventoryEntryWidget(item);
	}
	else {
		refreshInventoryEntryWidget(item);
	}
	return popped;
}

TObjectPtr<UItemInstance> UActorInventory::spawnItem(UItem* itemType, int quantity)
{
	TObjectPtr<UItemInstance> item = itemType->spawn(GetWorld(), quantity);
	addItem(item);
	return item;
}


void UActorInventory::clearInventory()
{
	// It's not necessary to set item->Owner=nullptr because the object instances are going to be garbage collected anyway.
	Items.Empty();
}

void UActorInventory::resetInventory()
{
	clearInventory();
	Loot->sample(this, 1);
}

void UActorInventory::refreshInventoryEntryWidget(TObjectPtr<UItemInstance> item) {
	check(item->Owner == this);
	if (InventoryWidget != nullptr) {
		InventoryWidget->updateItem(item);
	}
}

void UActorInventory::removeInventoryEntryWidget(TObjectPtr<UItemInstance> item)
{
	check(item->Owner == this);
	if (InventoryWidget != nullptr) {
		InventoryWidget->removeItem(item);
	}
}
void UActorInventory::addInventoryEntryWidget(TObjectPtr<UItemInstance> item)
{
	check(item->Owner == this);
	if (InventoryWidget != nullptr) {
		InventoryWidget->addItem(item);
	}
}