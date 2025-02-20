// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perk.generated.h"

class AGameCharacter;
struct FHealth;

UCLASS(BlueprintType)
class GAME_API UPerk : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual void BeginEffect(FHealth* health) const {};
	virtual void EndEffect(FHealth* health) const {};
};
