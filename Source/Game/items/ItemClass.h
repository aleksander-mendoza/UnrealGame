// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemClass.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class EItemClass : uint8
{
	PROP UMETA(DisplayName = "prop"),
	DOUBLE_HANDED_WEAPON  UMETA(DisplayName = "double handed weapon"),
	SINGLE_HANDED_WEAPON UMETA(DisplayName = "single handed weapon"),
	BOW UMETA(DisplayName = "bow"),
	SPEAR UMETA(DisplayName = "spear"),
	SINGLE_HANDED_QUIET_WEAPON UMETA(DisplayName = "single handed quiet weapon"),
	SHIELD UMETA(DisplayName = "shield"),
	FEMALE_CLOTHES UMETA(DisplayName = "female clothes"),
	MALE_CLOTHES UMETA(DisplayName = "male clothes"),
	FEMALE_LIGHT_ARMOR UMETA(DisplayName = "female light armor"),
	MALE_LIGHT_ARMOR UMETA(DisplayName = "male light armor"),
	FEMALE_HEAVY_ARMOR UMETA(DisplayName = "female heavy armor"),
	MALE_HEAVY_ARMOR UMETA(DisplayName = "male heavy armor"),
	BOOK UMETA(DisplayName = "book"),
	POTION UMETA(DisplayName = "potion"),
	FOOD UMETA(DisplayName = "food"),
	KEY UMETA(DisplayName = "food"),
	NONE UMETA(DisplayName = "placeholder when no item exists"),
	
};
inline bool isDoubleHanded(EItemClass c){
	return c == EItemClass::DOUBLE_HANDED_WEAPON || c == EItemClass::BOW;
}
inline bool isWearable(EItemClass c) {
	return c == EItemClass::FEMALE_CLOTHES
		|| c == EItemClass::MALE_CLOTHES
		|| c == EItemClass::FEMALE_LIGHT_ARMOR
		|| c == EItemClass::MALE_LIGHT_ARMOR
		|| c == EItemClass::FEMALE_HEAVY_ARMOR
		|| c == EItemClass::MALE_HEAVY_ARMOR;
}
inline bool isFemaleWearable(EItemClass c) {
	return c == EItemClass::FEMALE_CLOTHES
		|| c == EItemClass::FEMALE_LIGHT_ARMOR
		|| c == EItemClass::FEMALE_HEAVY_ARMOR;
}
inline bool isMaleWearable(EItemClass c) {
	return c == EItemClass::MALE_CLOTHES
		|| c == EItemClass::MALE_LIGHT_ARMOR
		|| c == EItemClass::MALE_HEAVY_ARMOR;
}