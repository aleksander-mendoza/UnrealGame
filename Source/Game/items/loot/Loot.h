// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Loot.generated.h"

class UActorInventory;
/**
 *
 */
UCLASS(BlueprintType)
class GAME_API ULoot : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void sample(UActorInventory* inv, int count=1) {}

	
};

