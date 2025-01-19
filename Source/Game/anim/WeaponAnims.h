// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAnims.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FWeaponAnims 
{
	GENERATED_BODY()



	/** left-handed attack combo Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UAnimMontage>> LeftHandedAnim;

	/** left-handed attack combo Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UAnimMontage>> RightHandedAnim;


	TArray<TObjectPtr<UAnimMontage>> & GetAnims(bool leftHand) {
		return leftHand ? LeftHandedAnim : RightHandedAnim;
	}
	TObjectPtr<UAnimMontage> GetAnim(bool leftHand, int idx) {
		TArray<TObjectPtr<UAnimMontage>>& anims = GetAnims(leftHand);
		if (anims.Num() > 0) {
			return anims[idx % anims.Num()];
		}
		return nullptr;
	}
	
};
