// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemObject.h"

/**
 * 
 */
class ContainerEvents
{
public:
	virtual ~ContainerEvents() {}
	virtual void OnEquipBothHands(TObjectPtr < UItemObject > item) = 0;
	virtual void OnEquipLeftHand(TObjectPtr < UItemObject > item) = 0;
	virtual void OnEquipRightHand(TObjectPtr < UItemObject > item) = 0;
	virtual void OnUnequipBothHands(TObjectPtr < UItemObject > item) = 0;
	virtual void OnUnequipLeftHand(TObjectPtr < UItemObject > item) = 0;
	virtual void OnUnequipRightHand(TObjectPtr < UItemObject > item) = 0;
	virtual void OnEquipClothes(TObjectPtr < UItemObject > item)=0;
	virtual void OnUnequipClothes(TObjectPtr < UItemObject > item) = 0;
	virtual void OnPutItem(TObjectPtr<UItemObject> item) = 0;
	virtual void OnDropItem(TObjectPtr<UItemObject> item) = 0;
	/**returns true if the clothing item is wearable and matches the gender/race of this character*/
	virtual bool CanWear(const FItem& item) = 0;
};
