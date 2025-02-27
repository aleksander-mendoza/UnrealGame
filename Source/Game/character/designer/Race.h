// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gender.h"
#include "Morph.h"
#include "Race.generated.h"


/**
 *
 */
UCLASS(BlueprintType)
class GAME_API URace : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGender Male;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGender Female;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (RowType = "Morph"))
	TSoftObjectPtr<UDataTable> Morphs;

	inline const FGender & getGender(bool isFemale) const{
		return isFemale ? Female : Male;
	}

	inline USkeletalMesh * getBodyMesh(bool isFemale) const {
		return getGender(isFemale).getBodyMesh();
	}
	inline class TSubclassOf<UAnimInstance> getAnimClass(bool isFemale) const {
		return getGender(isFemale).AnimClass;
	}
};

