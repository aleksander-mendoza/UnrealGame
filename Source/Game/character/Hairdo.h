// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hairdo.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FHairdo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> Mesh;

	inline USkeletalMesh* getSkeletalMesh() const{
		return Mesh.LoadSynchronous();
	}
};
