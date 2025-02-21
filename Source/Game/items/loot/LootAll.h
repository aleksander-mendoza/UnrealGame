// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "structs/LootItem.h"
#include "Loot.h"
#include "LootAll.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API ULootAll : public ULoot
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FLootItem> Items;

	
	virtual void sample(UActorInventory* inv, int count) override;

	
};

