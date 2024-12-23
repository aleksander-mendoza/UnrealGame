// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Warmth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Durbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ClothingSlots;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* Mesh;

};
