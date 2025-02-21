// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LootItemChance.generated.h"

class ULoot;
USTRUCT(BlueprintType)
struct FLootItemChance {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULoot* Item;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Likelihood=1;

	
};

