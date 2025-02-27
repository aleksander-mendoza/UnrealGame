// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hairdo.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FHairdo 
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool SimulatePhysics;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> Mesh;

	inline USkeletalMesh* getSkeletalMesh() const{
		return Mesh.LoadSynchronous();
	}
};
