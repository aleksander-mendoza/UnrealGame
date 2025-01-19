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


	/** Single-handed fighting animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnims DoubleHanded;



	FWeaponAnims & getAttackAnim(EMeleeAttackClass attackClass) {
		switch (attackClass) {
		case EMeleeAttackClass::DOUBLE_HANDED_WEAPON:
			return DoubleHanded;
		case EMeleeAttackClass::SINGLE_HANDED_WEAPON:
			return SingleHanded;
		case EMeleeAttackClass::BARE_HANDED:
			return BareHands;
		default:
			check(false);
			return BareHands;
		}
	}
	inline TObjectPtr<UAnimMontage> getAttackAnim(EMeleeAttackClass attackClass, bool leftHand, int idx) {
		switch (attackClass) {
		case EMeleeAttackClass::DOUBLE_HANDED_WEAPON:
			return DoubleHanded.GetAnim(leftHand, idx);
		case EMeleeAttackClass::SINGLE_HANDED_WEAPON:
			return SingleHanded.GetAnim(leftHand, idx);
		case EMeleeAttackClass::BARE_HANDED:
			return BareHands.GetAnim(leftHand, idx);
		default:
			check(false);
		case EMeleeAttackClass::NONE:
			return nullptr;
		}
	}
	

};
