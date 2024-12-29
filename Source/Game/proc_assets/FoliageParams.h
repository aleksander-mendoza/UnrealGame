// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGenUtils.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FoliageType.h"
#include "MovingGrid.h"
#include "../blender/rand.h"
#include "FoliageParams.generated.h"


USTRUCT(BlueprintType)
struct GAME_API FFoliageParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float chunkSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int spawnRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FFoliageType> types;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovingGrid TerrainGrid;

	

	inline void initMesh(UObject* owner, USceneComponent* parent) {
		for (int i = 0; i < types.Num(); i++) {
			types[i].initMesh(owner, parent);
			types[i].idx = i;
		}
	}

	inline void reset() {
		TerrainGrid.resetGridSize();
		const int area = TerrainGrid.getArea();
		for (int i = 0; i < types.Num(); i++)types[i].resetCache(area);
	}

	inline void clearSection(int sectionIdx) {
		for (int i = 0; i < types.Num(); i++)types[i].clearSection(sectionIdx);
	}

	inline void addChunksAroundPlayer() {
		TerrainGrid.addChunksWithinRadius(spawnRadius);
	}


};
