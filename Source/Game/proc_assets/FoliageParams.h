// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FoliageParams.generated.h"

USTRUCT(BlueprintType)
struct FoliageChunk {
	GENERATED_BODY()

	UPROPERTY()
	TArray<int> instanceIndices;
};

USTRUCT(BlueprintType)
struct FoliageChunks {
	GENERATED_BODY()

	UPROPERTY()
	TArray<FoliageChunk> chunks;

	UPROPERTY()
	TArray<FoliageChunk> indexPermutation;
};
/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FFoliageParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool hasCollisions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool alignToNormal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float density;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh * Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int spawnRadius;

	UPROPERTY()
	UInstancedStaticMeshComponent* InstancedMesh=nullptr;

	UPROPERTY()
	FoliageChunks cache;


	void resetCache(int renderArea);
	void addInstance();
};
