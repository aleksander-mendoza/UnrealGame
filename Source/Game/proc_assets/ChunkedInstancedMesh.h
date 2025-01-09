// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGenUtils.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ChunkedInstancedMesh.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FChunkedInstancedMesh
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool hasCollisions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EComponentMobility::Type> Mobility= EComponentMobility::Static;
	

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool simulatePhysics;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> Mesh; 

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool updateNav = false;


	UPROPERTY()
	UInstancedStaticMeshComponent* InstancedMesh=nullptr;


	EntityChunks cache;

	inline UStaticMesh* getMesh() {
		if (!Mesh.IsValid()) {
			Mesh.LoadSynchronous();
		}
		return Mesh.Get();
	}

	inline void initMesh(UObject* owner, USceneComponent* parent) {
		if (InstancedMesh == nullptr) {
			InstancedMesh = NewObject<UInstancedStaticMeshComponent>(owner, UInstancedStaticMeshComponent::StaticClass());
			InstancedMesh->bDisableCollision = !hasCollisions;
			InstancedMesh->SetRemoveSwap();
			InstancedMesh->SetSimulatePhysics(simulatePhysics);
			InstancedMesh->SetStaticMesh(getMesh());
			InstancedMesh->SetMobility(Mobility);
			InstancedMesh->RegisterComponent();
			InstancedMesh->AttachToComponent(parent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
	inline int addInstance(int sectionIdx, FTransform t) {
		return cache.sections[sectionIdx].instanceTransforms.Add(t);
	}
	inline FPrimitiveInstanceId addAndLoadInstance(int sectionIdx, FTransform t) {
		EntityChunk& chunk = cache.sections[sectionIdx];
		chunk.instanceTransforms.Add(t);
		FPrimitiveInstanceId id = InstancedMesh->AddInstanceById(t);
		chunk.instanceIndices.Add(id);
		return id;
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
