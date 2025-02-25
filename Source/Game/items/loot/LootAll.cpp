// Fill out your copyright notice in the Description page of Project Settings.


#include "LootAll.h"
#include "../inv/ActorInventory.h"


void ULootAll::sample(UActorInventory* inv, int count)
{
	if (count > 0) {
		UWorld* world = inv->GetWorld();
		for (int i = 0; i < Items.Num(); i++) {
			FLootItem& item = Items[i];
			if (IsValid(item.Item)) {
				int qty = item.getQuantity();
				if (qty > 0) {
					item.Item->sample(inv, qty * count);
				}
			}
		}
	}
}
