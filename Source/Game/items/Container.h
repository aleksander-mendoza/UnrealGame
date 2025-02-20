// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.h"

/**
 * 
 */
class ContainerEvents
{
public:
	virtual ~ContainerEvents() {}
	virtual void OnEquipBothHands(TObjectPtr < UItemInstance > item) = 0;
	virtual void OnEquipLeftHand(TObjectPtr < UItemInstance > item) = 0;
	virtual void OnEquipRightHand(TObjectPtr < UItemInstance > item) = 0;
	virtual void OnUnequipBothHands(TObjectPtr < UItemInstance > item) = 0;
	virtual void OnUnequipLeftHand(TObjectPtr < UItemInstance > item) = 0;
	virtual void OnUnequipRightHand(TObjectPtr < UItemInstance > item) = 0;
	virtual void OnEquipClothes(TObjectPtr < UItemInstance > item)=0;
	virtual void OnUnequipClothes(TObjectPtr < UItemInstance > item) = 0;
	virtual void OnPutItem(TObjectPtr<UItemInstance> item) = 0;
	virtual void OnDropItem(TObjectPtr<UItemInstance> item) = 0;
};
