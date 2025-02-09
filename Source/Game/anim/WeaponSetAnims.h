// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAnims.h"
#include "MeleeAttackClass.h"
#include "WeaponSetAnims.generated.h"


USTRUCT(BlueprintType)
struct GAME_API FWeaponSetAnims 
{
	GENERATED_BODY()


	/** Bare handed fighting animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnims BareHands;

	/** Single-handed fighting animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnims SingleHanded;

	/** Bow animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnims Bow;

	/** Single-handed fighting animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnims DoubleHanded;


	FWeaponAnim * getSheathAnim(EMeleeAttackClass attackClass) {
		switch (attackClass) {
		case EMeleeAttackClass::DOUBLE_HANDED_WEAPON:
			return DoubleHanded.Sheath.getItself();
		case EMeleeAttackClass::SINGLE_HANDED_WEAPON:
			return SingleHanded.Sheath.getItself();
		case EMeleeAttackClass::BARE_HANDED:
			return BareHands.Sheath.getItself();
		default:
			check(false);
		case EMeleeAttackClass::NONE:
			return nullptr;
		}
	}
	FWeaponAnim* getUnsheathAnim(EMeleeAttackClass attackClass) {
		switch (attackClass) {
		case EMeleeAttackClass::DOUBLE_HANDED_WEAPON:
			return DoubleHanded.Unsheath.getItself();
		case EMeleeAttackClass::SINGLE_HANDED_WEAPON:
			return SingleHanded.Unsheath.getItself();
		case EMeleeAttackClass::BARE_HANDED:
			return BareHands.Unsheath.getItself();
		default:
			check(false);
		case EMeleeAttackClass::NONE:
			return nullptr;
		}
	}
	FWeaponAnims * getAttackAnims(EMeleeAttackClass attackClass) {
		switch (attackClass) {
		case EMeleeAttackClass::DOUBLE_HANDED_WEAPON:
			return &DoubleHanded;
		case EMeleeAttackClass::SINGLE_HANDED_WEAPON:
			return &SingleHanded;
		case EMeleeAttackClass::BOW:
			return &Bow;
		default:
			check(false);
		case EMeleeAttackClass::BARE_HANDED:
			return &BareHands;
		case EMeleeAttackClass::NONE:
			return nullptr;
		}
	}
	inline FWeaponAnim* getAttackAnim(EMeleeAttackClass attackClass, int idx) {
		switch (attackClass) {
		case EMeleeAttackClass::DOUBLE_HANDED_WEAPON:
			return DoubleHanded.GetAnim(idx);
		case EMeleeAttackClass::SINGLE_HANDED_WEAPON:
			return SingleHanded.GetAnim(idx);
		case EMeleeAttackClass::BARE_HANDED:
			return BareHands.GetAnim(idx);
		default:
			check(false);
		case EMeleeAttackClass::NONE:
			return nullptr;
		}
	}
	

};
