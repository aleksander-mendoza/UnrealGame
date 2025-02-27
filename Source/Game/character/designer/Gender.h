// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hairdo.h"
#include "Gender.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FGender
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> BodyMesh;


	/* The AnimBlueprint class to use. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class TSubclassOf<UAnimInstance> AnimClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FHairdo> Hairdos;

	inline USkeletalMesh* getBodyMesh() const{
		return BodyMesh.LoadSynchronous();
	}
};
