// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "structs/LootItemChance.h"
#include "Loot.h"
#include "LootProbabilistic.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API ULootProbabilistic : public ULoot
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MinQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MaxQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FLootItemChance> Items;

	
	virtual void sample(UActorInventory* inv, int count) override;

	
};

