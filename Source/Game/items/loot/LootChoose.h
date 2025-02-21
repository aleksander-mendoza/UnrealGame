// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../item/Item.h"
#include "Loot.h"
#include "LootChoose.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API ULootChoose : public ULoot
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MinQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MaxQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSoftObjectPtr<ULoot>> Items;

	
	virtual void sample(UActorInventory* inv, int count) override;

	
};

