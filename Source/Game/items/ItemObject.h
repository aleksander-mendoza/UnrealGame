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

	// -4 = held in both hands
	// -3 = not equipped
	// -2 = held in left hand
	// -1 = held in right hand
	// n>=0 = equipped as clothes at n-th index in clothes stack
	UPROPERTY(BlueprintReadOnly)
	int equippedAt = -3;

	class UActorInventory* container=nullptr;
	inline USkeletalMesh* getSkeletalMesh() {
		return Instance.getSkeletalMesh();
	}
	inline UStaticMesh* getMesh() {
		return Instance.getMesh();
	}
	inline FItem* getRow() {
		return Instance.getRow();
	}
	inline EItemClass getItemClass() {
		return Instance.getRow()->Class;
	}
	inline float getItemDamage() {
		return Instance.getRow()->Damage;
	}
	inline float getItemArmor() {
		return Instance.getRow()->Armor;
	}
	inline float getItemWeight() {
		return Instance.getRow()->Weight;
	}
	inline float isWearable() {
		return Instance.getRow()->isWearable();
	}
};
