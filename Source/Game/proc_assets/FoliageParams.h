// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FoliageParams.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FFoliageParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool alignToNormal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float density;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh * Mesh;

};
