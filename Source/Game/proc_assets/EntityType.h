// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkedInstancedMesh.h"
#include "EntityType.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FEntityType
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool alignToNormal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float density;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool treatDensityAsMaxCount=false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float loadRadius=-1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float unloadRadius = -1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int seed=-1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FChunkedInstancedMesh chunked;

	inline void initMesh(UObject* owner, USceneComponent* parent) {
		unloadRadius = math::max(loadRadius + 1, unloadRadius);
		chunked.initMesh(owner, parent);
		
	}
	inline int addInstance(int sectionIdx, FTransform t) {
		return chunked.addInstance(sectionIdx, t);
	}
	inline FPrimitiveInstanceId addAndLoadInstance(int sectionIdx, FTransform t) {
		return chunked.addAndLoadInstance(sectionIdx, t);
	}
	inline void resetCache(int renderArea)
	{
		chunked.resetCache(renderArea);
	}
	inline void reload(int sectionIdx)
	{
		chunked.reload(sectionIdx);
	}
	inline void reload(EntityChunk& chunk)
	{
		chunked.reload(chunk);
	}
	inline void loadUnloadSection(int sectionIdxToLoad, int sectionIdxToUnload)
	{
		chunked.loadUnloadSection(sectionIdxToLoad, sectionIdxToUnload);
	}
	
	inline bool loadSection(int sectionIdx)
	{
		return chunked.loadSection(sectionIdx);
	}

	
	inline bool unloadSection(int sectionIdx)
	{
		return chunked.unloadSection(sectionIdx);
	}
	inline EntityChunk& clearSection(int sectionIdx)
	{
		return chunked.clearSection(sectionIdx);
	}
};
