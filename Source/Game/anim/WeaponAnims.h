// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAnim.h"
#include "WeaponAnims.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FWeaponAnims 
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnim Sheath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnim Unsheath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWeaponAnim> AttackAnims;



	FWeaponAnim * GetAnim(int idx) {
		if (AttackAnims.Num() > 0) {
			return AttackAnims[idx % AttackAnims.Num()].getItself();
		}
		return nullptr;
	}
	
};
