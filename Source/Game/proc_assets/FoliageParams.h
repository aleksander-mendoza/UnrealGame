// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGenUtils.h"
#include "Logging/StructuredLog.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FoliageParams.generated.h"

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
	float spawnRadius;

	UPROPERTY()
	UInstancedStaticMeshComponent* InstancedMesh=nullptr;

	
	FoliageChunks cache;

	inline void initMesh(UObject* owner, USceneComponent* parent) {
		InstancedMesh = NewObject<UInstancedStaticMeshComponent>(owner, UInstancedStaticMeshComponent::StaticClass(), TEXT("FoliageMesh"));
		InstancedMesh->bDisableCollision = !hasCollisions;
		InstancedMesh->SetRemoveSwap();
		InstancedMesh->SetStaticMesh(Mesh);
		InstancedMesh->RegisterComponent();
		InstancedMesh->AttachToComponent(parent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	inline void addInstance(int sectionIdx, FTransform t) {
		cache.sections[sectionIdx].instanceTransforms.Add(t);
	}
	inline void resetCache(int renderArea)
	{
		cache.sections.Empty();
		cache.sections.Reserve(renderArea);
		while (cache.sections.Num() < renderArea) {
			cache.sections.AddDefaulted();
		}
		check(cache.sections.Num() == renderArea);
	}
	inline void loadUnloadSection(int sectionIdxToLoad, int sectionIdxToUnload)
	{
		FoliageChunk& chunkToUnload = cache.sections[sectionIdxToUnload];
		FoliageChunk& chunkToLoad = cache.sections[sectionIdxToLoad];
		check(chunkToUnload.isLoaded);
		check(!chunkToLoad.isLoaded);
		UE_LOGFMT(LogCore, Warning, "Loaded {0} unloaded {1}", sectionIdxToLoad, sectionIdxToUnload);
		std::swap(chunkToLoad.instanceIndices, chunkToUnload.instanceIndices);
		const int numToUpdate = math::min(chunkToUnload.instanceTransforms.Num(), chunkToLoad.instanceTransforms.Num());
		for (int i = 0; i < numToUpdate;i++) {
			InstancedMesh->UpdateInstanceTransformById(chunkToLoad.instanceIndices[i],chunkToLoad.instanceTransforms[i]);
		}
		if (chunkToLoad.instanceIndices.Num() > chunkToLoad.instanceTransforms.Num()) {
			TArrayView<FPrimitiveInstanceId> view = chunkToLoad.instanceIndices;
			view.RightChopInline(chunkToLoad.instanceTransforms.Num());
			InstancedMesh->RemoveInstancesById(view, false);
			chunkToLoad.instanceIndices.SetNum(chunkToLoad.instanceTransforms.Num());
		}
		else if (chunkToLoad.instanceIndices.Num() < chunkToLoad.instanceTransforms.Num()) {
			TArrayView<FTransform> view = chunkToLoad.instanceTransforms;
			view.RightChopInline(chunkToLoad.instanceIndices.Num());
			chunkToLoad.instanceIndices += InstancedMesh->AddInstancesById(view, false);
		}
		chunkToUnload.isLoaded = false;
		chunkToLoad.isLoaded = true;
	}
	
	inline bool loadSection(int sectionIdx)
	{
		FoliageChunk& chunk = cache.sections[sectionIdx];
		if (!chunk.isLoaded) {
			UE_LOGFMT(LogCore, Warning, "Loaded {0}", sectionIdx);
			chunk.instanceIndices = InstancedMesh->AddInstancesById(chunk.instanceTransforms, false);
			chunk.isLoaded = true;
			return true;
		}
		return false;
	}

	
	inline bool unloadSection(int sectionIdx)
	{
		FoliageChunk& chunk = cache.sections[sectionIdx];
		if (chunk.isLoaded) {
			UE_LOGFMT(LogCore, Warning, "Unloaded {0}", sectionIdx);
			InstancedMesh->RemoveInstancesById(chunk.instanceIndices, false);
			chunk.instanceIndices.Empty();
			chunk.isLoaded = false;
			return true;
		}
		return false;
	}
	inline FoliageChunk& clearSection(int sectionIdx)
	{
		FoliageChunk& chunk = cache.sections[sectionIdx];
		chunk.instanceTransforms.Empty();
		return chunk;
	}
};
