// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LootItem.generated.h"

class ULoot;

USTRUCT(BlueprintType)
struct FLootItem {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULoot * Item;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MinQuantity=1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MaxQuantity = 1;

	inline int getQuantity() const {
		return FMath::RandRange(MinQuantity, MaxQuantity);
	}
	
};

