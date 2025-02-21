// Fill out your copyright notice in the Description page of Project Settings.


#include "LootChoose.h"
#include "../inv/ActorInventory.h"


void ULootChoose::sample(UActorInventory* inv, int count)
{
	if (count > 0 && Items.Num() > 0) {
		UWorld* world = inv->GetWorld();
		int quantity = FMath::RandRange(MinQuantity, MaxQuantity);
		for (int i = 0; i < quantity; i++) {
			int idx = FMath::RandRange(0, Items.Num() - 1);
			TSoftObjectPtr<ULoot> item = Items[idx];
			ULoot * loaded = item.LoadSynchronous();
			loaded->sample(inv, count);
		}
	}
}
