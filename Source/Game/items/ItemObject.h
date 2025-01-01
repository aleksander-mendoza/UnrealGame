// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.h"
#include "ItemObject.generated.h"

UCLASS()
class GAME_API UItemObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemInstance Instance;

	// -3 = not equipped
	// -2 = held in left hand
	// -1 = held in right hand
	// n>=0 = equipped as clothes at n-th index in clothes stack
	UPROPERTY(BlueprintReadOnly)
	int equippedAt = -1;
};
