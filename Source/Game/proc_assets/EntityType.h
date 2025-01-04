// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGenUtils.h"
#include "../items/ItemActor.h"
#include "Logging/StructuredLog.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "EntityType.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FEntityType
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
	TEnumAsByte<EComponentMobility::Type> Mobility= EComponentMobility::Static;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool treatDensityAsMaxCount=false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool simulatePhysics;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh * Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float loadRadius=-1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool updateNav = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float unloadRadius = -1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int seed=-1;

	UPROPERTY()
	UInstancedStaticMeshComponent* InstancedMesh=nullptr;


	EntityChunks cache;

	inline void initMesh(UObject* owner, USceneComponent* parent) {
		unloadRadius = math::max(loadRadius + 1, unloadRadius);
		InstancedMesh = NewObject<UInstancedStaticMeshComponent>(owner, UInstancedStaticMeshComponent::StaticClass());
		InstancedMesh->bDisableCollision = !hasCollisions;
		InstancedMesh->SetRemoveSwap();
		InstancedMesh->SetSimulatePhysics(simulatePhysics);
		InstancedMesh->SetStaticMesh(Mesh);
		InstancedMesh->SetMobility(Mobility);
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
	inline void reload(int sectionIdx)
	{
		reload(cache.sections[sectionIdx]);
	}
	inline void reload(EntityChunk& chunk)
	{
		const int numToUpdate = math::min(chunk.instanceIndices.Num(), chunk.instanceTransforms.Num());
		for (int i = 0; i < numToUpdate; i++) {
			InstancedMesh->UpdateInstanceTransformById(chunk.instanceIndices[i], chunk.instanceTransforms[i]);
		}
		if (chunk.instanceIndices.Num() > chunk.instanceTransforms.Num()) {
			TArrayView<FPrimitiveInstanceId> view = chunk.instanceIndices;
			view.RightChopInline(chunk.instanceTransforms.Num());
			InstancedMesh->RemoveInstancesById(view, updateNav);
			chunk.instanceIndices.SetNum(chunk.instanceTransforms.Num());
		}
		else if (chunk.instanceIndices.Num() < chunk.instanceTransforms.Num()) {
			TArrayView<FTransform> view = chunk.instanceTransforms;
			view.RightChopInline(chunk.instanceIndices.Num());
			chunk.instanceIndices += InstancedMesh->AddInstancesById(view, updateNav);
		}
		chunk.isDirty = false;
	}
	inline void loadUnloadSection(int sectionIdxToLoad, int sectionIdxToUnload)
	{
		EntityChunk& chunkToUnload = cache.sections[sectionIdxToUnload];
		EntityChunk& chunkToLoad = cache.sections[sectionIdxToLoad];
		check(chunkToUnload.isLoaded);
		check(!chunkToLoad.isLoaded);
		UE_LOGFMT(LogCore, Warning, "Loaded {0} unloaded {1} (seed={2})", sectionIdxToLoad, sectionIdxToUnload, seed);
		check(chunkToLoad.instanceIndices.IsEmpty());
		std::swap(chunkToLoad.instanceIndices, chunkToUnload.instanceIndices);
		reload(chunkToLoad);
		chunkToUnload.isLoaded = false;
		chunkToUnload.isDirty = false;
		chunkToLoad.isLoaded = true;
	}
	
	inline bool loadSection(int sectionIdx)
	{
		EntityChunk& chunk = cache.sections[sectionIdx];
		if (chunk.isDirty) {
			reload(chunk);
		}
		else if (!chunk.isLoaded) {
			UE_LOGFMT(LogCore, Warning, "Loaded {0} (seed={1})", sectionIdx, seed);
			check(chunk.instanceIndices.IsEmpty());
			chunk.instanceIndices = InstancedMesh->AddInstancesById(chunk.instanceTransforms, updateNav);
			chunk.isLoaded = true;
			return true;
		}
		check(chunk.instanceIndices.Num() == chunk.instanceTransforms.Num());
		return false;
	}

	
	inline bool unloadSection(int sectionIdx)
	{
		EntityChunk& chunk = cache.sections[sectionIdx];
		if (chunk.isLoaded) {
			UE_LOGFMT(LogCore, Warning, "Unloaded {0} (seed={1})", sectionIdx, seed);
			InstancedMesh->RemoveInstancesById(chunk.instanceIndices, updateNav);
			chunk.instanceIndices.Empty();
			chunk.isLoaded = false;
			chunk.isDirty = false;
			return true;
		}
		return false;
	}
	inline EntityChunk& clearSection(int sectionIdx)
	{
		EntityChunk& chunk = cache.sections[sectionIdx];
		chunk.instanceTransforms.Empty();
		chunk.isDirty = chunk.isLoaded;
		return chunk;
	}
};
