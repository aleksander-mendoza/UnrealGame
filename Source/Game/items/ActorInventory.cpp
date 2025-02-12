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

bool UActorInventory::equipItem(TObjectPtr<UItemObject> item, bool leftHand, bool unequipClothesWithOverlappingSlots)
{
	const FItem* meta = item->Instance.getRow();
	const EItemClass itemClass = meta->Class;
	switch (itemClass) {
		case EItemClass::PROP:
			break;
		case EItemClass::DOUBLE_HANDED_WEAPON:
		case EItemClass::SINGLE_HANDED_WEAPON:
		case EItemClass::BOW:
		case EItemClass::SPEAR:
		case EItemClass::SINGLE_HANDED_QUIET_WEAPON:
		case EItemClass::SHIELD: {
			takeInHand(item, leftHand);
			return true;
		}
		case EItemClass::FEMALE_CLOTHES:
		case EItemClass::MALE_CLOTHES:
		case EItemClass::FEMALE_LIGHT_ARMOR:
		case EItemClass::MALE_LIGHT_ARMOR:
		case EItemClass::FEMALE_HEAVY_ARMOR:
		case EItemClass::MALE_HEAVY_ARMOR: {
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
		case EItemClass::PROJECTILE: {
			selectProjectile(item);
			return true;
		}
		case EItemClass::BOOK:
			break;
		case EItemClass::POTION:
			break;
		case EItemClass::FOOD:
			break;
		case EItemClass::KEY:
			break;
		case EItemClass::NONE:
			break;
	}
	return false;
}

bool UActorInventory::unequipItem(TObjectPtr<UItemObject> item, bool force)
{
	check(item->container == this);
	switch(item->equippedAt) {
	default:
		if (force)return removeClothingItem<false>(item);
		else return removeClothingItem<true>(item);
	case EQUIPPED_AT_RIGHT_HAND:
		check(item == RightHand);
		emptyRightHand();
		return true;
	case EQUIPPED_AT_LEFT_HAND:
		check(item == LeftHand);
		emptyLeftHand();
		return true;
	case EQUIPPED_AT_DOUBLEHANDED:
		check(item == RightHand);
		check(item == LeftHand);
		emptyBothHands();
		return true;
	case EQUIPPED_AT_PROJECTILE:
		check(SelectedProjectile == item);
		SelectedProjectile = nullptr;
		return true;
	case EQUIPPED_AT_NONE:
		check(false);
		break;
	}
	return false;
}




