// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"
#include "MovingGrid.h"
#include "EntityType.h"
#include "EntityParams.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FEntityParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEntityType> Types;

	/**This is the radius within which entities are going to be spawned (their potisions are loadedd into memory)
	but this is not the radius at which the entities become visible. Each entity type has their own individual load radius
	(radius within which the entities are actually loaded into the world)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float spawnRadius=-1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovingGrid SpawnGrid;

	

	inline void initMesh(UObject* owner, USceneComponent* parent, int idx) {
		//Types.Sort([](const FEntityType& ip1, const FEntityType& ip2) {
		//	return  ip1.spawnRadius > ip2.spawnRadius;
		//	});
		//check(Types.Num() == 0 || Types[0].spawnRadius >= Types.Last().spawnRadius);
		if (spawnRadius < 0) {
			spawnRadius = float(SpawnGrid.radius) - 1;
		}
		else {
			spawnRadius = math::min(spawnRadius, float(SpawnGrid.radius) - 1);
		}
		
		for (int i = 0; i < Types.Num(); i++) {
			FEntityType & t = Types[i];
			if (t.loadRadius < 0) {
				t.loadRadius = spawnRadius;
			}
			else {
				t.loadRadius = math::min(t.loadRadius, spawnRadius);
			}
			t.density = math::min(t.density, 0.01);
			if (t.seed == -1)t.seed = idx*500 + i;
			t.initMesh(owner, parent);
			check(t.unloadRadius <= SpawnGrid.radius);
		}
	}
	inline void addInstance(int typeIdx, int sectionIdx, FTransform t) {
		Types[typeIdx].addInstance(sectionIdx, t);
	}

	inline void reset(const double3 playerPosition) {
		SpawnGrid.setCenterPos(playerPosition);
		resetCache();
	}
	inline void resetCache()
	{
		SpawnGrid.reset();
		for (int i = 0; i < Types.Num(); i++)Types[i].resetCache(SpawnGrid.getArea());
	}
	template<bool l2>
	inline void addChunksAroundPlayer() {
		SpawnGrid.addChunksWithinRadius<l2>(spawnRadius);
	}
	inline void loadUnloadSection(int sectionIdxToLoad, int sectionIdxToUnload)
	{
		for (int i = 0; i < Types.Num(); i++)Types[i].loadUnloadSection(sectionIdxToLoad, sectionIdxToUnload);
	}
	
	inline void loadSection(int sectionIdx)
	{
		for (int i = 0; i < Types.Num(); i++)Types[i].loadSection(sectionIdx);
	}
	inline bool update(double3 playerPos) {
		return SpawnGrid.update(playerPos, spawnRadius);
	}
	
	inline void unloadSection(int sectionIdx)
	{
		for (int i = 0; i < Types.Num(); i++)Types[i].unloadSection(sectionIdx);
	}
	inline void clearSection(int sectionIdx)
	{
		for (int i = 0; i < Types.Num(); i++)Types[i].clearSection(sectionIdx);
	}
};
