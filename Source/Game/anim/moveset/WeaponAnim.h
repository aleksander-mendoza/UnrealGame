// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAnim.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FWeaponAnim
{
	GENERATED_BODY()

	

	/**If true then you cannot simulatneously play a second animation on the other hand. 
	The animation essentially engages both hands. That does not necessarily mean that the
	animation must be for two-handed weapons. There could be an animation for two 
	single-handed weapons wielded simulatenously or it could even be a simple single-haded animation
	that engages torso/both hands a lot and would look bad if some other animation played for the other hand*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool EngagesBothHands;

	/**True if the animation is driven by left hand. Even double-handed animations 
	have one hand as the dominant side. Depending on whether user clicks left or right mouse button, the
	animation will be flipped to make the dominant side match the mouse button.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsLeftHanded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Anim;



	
};
