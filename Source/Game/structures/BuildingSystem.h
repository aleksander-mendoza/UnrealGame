// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "../blender/utildefines.h"
#include "../proc_assets/MovingGrid.h"
#include "BuildingSystem.generated.h"


USTRUCT(BlueprintType)
struct GAME_API FBuildingSystem
{
	GENERATED_BODY()
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float tileSize; 


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int numTilesAlongChunkSide;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int loadRadius = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int unloadRadius = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int spawnRadius=-1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTile> Tiles;

	TileChunks PlacedTiles;
	FMovingGrid Grid;
	UObject* Owner; 
	USceneComponent* Parent;

	inline void init(UObject* owner, USceneComponent* parent) {
		Owner = owner;
		Parent = parent;
		unloadRadius = math::max(loadRadius + 1, unloadRadius);
		if (loadRadius < 0) {
			loadRadius = spawnRadius;
		}
		else {
			loadRadius = math::min(loadRadius, spawnRadius);
		}
		Grid.chunkSize = tileSize * numTilesAlongChunkSide;
	}
	inline TileMeta & putTile(const int tileIdx, const double3 position, const FRotator & rot) {
		const int3 tileDiscretePos = int3(math::floor(position / tileSize));
		const double3 tileContPos = double3(tileDiscretePos) * tileSize + tileSize/2;
		const int2 chunkAbsPos = Grid.getChunkAbsPosFromWorldPos(position);
		const int2 chunkRelPos = Grid.absToRelPos(chunkAbsPos);
		const int chunkIdx = Grid.getChunkIdx(chunkRelPos);
		const int sectionIdx = Grid.getSectionIdx(chunkIdx);
		FTile& t = Tiles[tileIdx];
		t.initMesh(Owner, Parent);
		TMap<int3, TileMeta>& chunkGrid = PlacedTiles.sections[sectionIdx].posToInstance;
		check(chunkGrid.Find(tileDiscretePos) == nullptr);
		FPrimitiveInstanceId id = t.addAndLoadInstance(sectionIdx, FTransform(rot, tileContPos));
		TileMeta & tm = chunkGrid.Add(tileDiscretePos);
		tm.id = id;
		tm.tileIdx = tileIdx;
		return tm;
	}
	inline void reset(const double3 playerPosition) {
		Grid.setCenterPos(playerPosition);
		resetCache();
	}
	inline void resetCache()
	{
		Grid.reset();
		for (int i = 0; i < Tiles.Num(); i++)Tiles[i].resetCache(Grid.getArea());
	}
	template<bool l2>
	inline void addChunksAroundPlayer() {
		Grid.addChunksWithinRadius<l2>(spawnRadius);
	}
	inline void loadUnloadSection(int sectionIdxToLoad, int sectionIdxToUnload)
	{
		for (int i = 0; i < Tiles.Num(); i++)Tiles[i].loadUnloadSection(sectionIdxToLoad, sectionIdxToUnload);
	}

	inline void loadSection(int sectionIdx)
	{
		for (int i = 0; i < Tiles.Num(); i++)Tiles[i].loadSection(sectionIdx);
	}
	inline bool update(double3 playerPos) {
		return Grid.update(playerPos, spawnRadius);
	}

	inline void unloadSection(int sectionIdx)
	{
		for (int i = 0; i < Tiles.Num(); i++)Tiles[i].unloadSection(sectionIdx);
	}
	inline void clearSection(int sectionIdx)
	{
		for (int i = 0; i < Tiles.Num(); i++)Tiles[i].clearSection(sectionIdx);
	}

};