// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorInventory.h"

// Sets default values for this component's properties
UActorInventory::UActorInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


void UActorInventory::PostInitProperties()
{
	Super::PostInitProperties();
	
}

// Called when the game starts
void UActorInventory::BeginPlay()
{
	Super::BeginPlay();

	
}


// Called every frame
void UActorInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

bool UActorInventory::equipItem(UItemObject* item, bool leftHand, bool unequipClothesWithOverlappingSlots)
{
	const FItem* meta = item->Instance.getRow();
	if (meta->isWearable()) {
		if (containerEvents && !containerEvents->CanWear(*meta)) return false;
		if (!canPutOn(*meta)) {
			if (unequipClothesWithOverlappingSlots && canMakeSpaceFor(*meta)) {
				const bool r = makeSpaceFor<true>(*meta);
				check(r);
			}
			else {
				return false;
			}
		}
		const bool r = addClothingItem<true>(item);
		check(r);
		return true;
	}
	else {
		takeInHand(item, leftHand);
		return true;
	}
	
}

bool UActorInventory::unequipItem(UItemObject* item, bool force)
{
	check(item->container == this);
	if (item->equippedAt >= 0) {
		if (force)return removeClothingItem<false>(item);
		else return removeClothingItem<true>(item);
	}
	else if (item->equippedAt == -1) {
		check(item == RightHand);
		emptyRightHand();
		return true;
	}
	else if (item->equippedAt == -2) {
		check(item == LeftHand);
		emptyLeftHand();
		return true;
	}
	else if (item->equippedAt == -4) {
		check(item == RightHand);
		check(item == LeftHand);
		emptyBothHands();
		return true;
	}
	return false;
}




