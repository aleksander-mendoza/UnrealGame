// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ItemInstance.generated.h"

USTRUCT(BlueprintType)
struct GAME_API FItemInstance : public FTableRowBase
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="Item"))
	FDataTableRowHandle Item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Quantity;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Durbility;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int EnchantmentEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int EnchantmentLevel;

	


	inline FItem* getRow() const{
		return Item.GetRow<FItem>("");
	}

	inline USkeletalMesh* getSkeletalMesh() {
		return getRow()->getSkeletalMesh();
	}
	inline UStaticMesh* getMesh() {
		return getRow()->getMesh();
	}
};
