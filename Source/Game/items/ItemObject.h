// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.h"
#include "ItemObject.generated.h"
 
#define EQUIPPED_AT_PROJECTILE -5
#define EQUIPPED_AT_DOUBLEHANDED -4
#define EQUIPPED_AT_NONE -3
#define EQUIPPED_AT_LEFT_HAND -2
#define EQUIPPED_AT_RIGHT_HAND -1
UCLASS()
class GAME_API UItemObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemInstance Instance;

	// -5 = selected as projectile
	// -4 = held in both hands
	// -3 = not equipped
	// -2 = held in left hand
	// -1 = held in right hand
	// n>=0 = equipped as clothes at n-th index in clothes stack
	UPROPERTY(BlueprintReadOnly)
	int equippedAt = EQUIPPED_AT_NONE;


	class UActorInventory* container=nullptr;

	
	inline USkeletalMesh* getSkeletalMesh() {
		return Instance.getSkeletalMesh();
	}
	inline UStaticMesh* getMesh() {
		return Instance.getMesh();
	}
	inline FItem* getRow() const {
		return Instance.getRow();
	}
	inline bool isLeftTheDominantHand() {
		return Instance.getRow()->isLeftTheDominantHand();
	}
	inline EItemClass getItemClass() const {
		return Instance.getRow()->Class;
	}
	inline float getItemDamage() const {
		return Instance.getRow()->Damage;
	}
	inline float getItemArmor() const {
		return Instance.getRow()->Armor;
	}
	inline float getItemWeight()const {
		return Instance.getRow()->Weight;
	}
	inline float isWearable() const {
		return Instance.getRow()->isWearable();
	}
	inline bool isSheathedOnTheBack() {
		return Instance.getRow()->isSheathedOnTheBack();
	}
};
