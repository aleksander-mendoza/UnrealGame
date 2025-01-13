// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../items/ItemInstance.h"
#include "InventoryItemObject.generated.h"

UCLASS()
class GAME_API UInventoryItemObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemInstance Instance;
};
