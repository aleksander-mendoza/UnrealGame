// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGenUtils.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FoliageType.generated.h"

USTRUCT(BlueprintType)
struct GAME_API FFoliageType
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
	UStaticMesh* Mesh;

	UPROPERTY()
	UInstancedStaticMeshComponent* InstancedMesh = nullptr;

	FoliageChunks cache;
	int idx;
	

	inline void initMesh(UObject* owner, USceneComponent * parent) {
		InstancedMesh = NewObject<UInstancedStaticMeshComponent>(owner, UInstancedStaticMeshComponent::StaticClass(), TEXT("FoliageMesh"));
		InstancedMesh->bDisableCollision = !hasCollisions;
		InstancedMesh->SetRemoveSwap();
		InstancedMesh->SetStaticMesh(Mesh);
		InstancedMesh->RegisterComponent();
		InstancedMesh->AttachToComponent(parent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
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
	inline bool validateCache() const {
		for (int instanceIdx = 0; instanceIdx < cache.inverseLookup.Num(); instanceIdx++) {
			const short2 s = cache.inverseLookup[instanceIdx];
			const int i = cache.sections[s.x].instanceIndices[s.y];
			check(i == instanceIdx);
		}
		int sum = 0;
		for (int i = 0; i < cache.sections.Num(); i++) {
			sum += cache.sections[i].instanceIndices.Num();
		}
		return sum == cache.inverseLookup.Num();
	}
	inline int addInstances(int sectionIdx, TArray<FTransform> instances, int batchSize)
	{
		batchSize = math::min(batchSize, instances.Num());
		const int startInstanceIdx = InstancedMesh->GetNumInstances();
		InstancedMesh->AddInstances(instances,false,true,false);
		for (int instanceIdx = startInstanceIdx; instanceIdx < startInstanceIdx+ batchSize; instanceIdx++) {
			const int idxWithinChunk = cache.sections[sectionIdx].instanceIndices.Add(instanceIdx);
			cache.inverseLookup.Add(short2(sectionIdx, idxWithinChunk));
		}
	}
	inline int addInstance(int sectionIdx, FTransform transform)
	{
		check(InstancedMesh != nullptr);
		check(sectionIdx >= 0);
		check(sectionIdx < cache.sections.Num());

		//InstancedMesh->BatchUpdateInstancesData
		const int instanceIdx = InstancedMesh->AddInstance(transform, true);
		const int idxWithinChunk = cache.sections[sectionIdx].instanceIndices.Add(instanceIdx);
		cache.inverseLookup.Add(short2(sectionIdx, idxWithinChunk));
		//check(validateCache()); // this should always be true but it's a quite heavy check. Use only when debugging
		return instanceIdx;

	}

	inline void removeInstanceSwap(int sectionIdx, int idxWithinChunk)
	{
		//check(validateCache());
		check(InstancedMesh != nullptr);
		check(sectionIdx >= 0);
		check(sectionIdx < cache.sections.Num());
		FoliageChunk& chunk = cache.sections[sectionIdx];
		const int instanceIdx = chunk.instanceIndices[idxWithinChunk];
		const short2 mostRecentInstanceInverseIdx = cache.inverseLookup.Last();
		cache.inverseLookup[instanceIdx] = mostRecentInstanceInverseIdx;
		int& mostRecentlyAddedInstanceIdx = cache.sections[mostRecentInstanceInverseIdx.x].instanceIndices[mostRecentInstanceInverseIdx.y];
		//check(mostRecentlyAddedInstanceIdx != instanceIdx);
		mostRecentlyAddedInstanceIdx = instanceIdx;
		cache.inverseLookup.Pop();
		InstancedMesh->RemoveInstance(instanceIdx);
		chunk.instanceIndices.RemoveAtSwap(idxWithinChunk);
		//check(validateCache());
	}
	inline void clearSection(int sectionIdx)
	{
		FoliageChunk& chunk = cache.sections[sectionIdx];
		if (chunk.isPopulated) {
			for (int idxWithinChunk = chunk.instanceIndices.Num() - 1; idxWithinChunk >= 0; idxWithinChunk--) {
				removeInstanceSwap(sectionIdx, idxWithinChunk);
			}
			/*
			for (int idxWithinChunk = chunk.instanceIndices.Num() - 1; idxWithinChunk >= 0; idxWithinChunk--) {
				const int instanceIdx = chunk.instanceIndices[idxWithinChunk];
				const short2 mostRecentInstanceInverseIdx = cache.inverseLookup.Last();
				cache.inverseLookup[instanceIdx] = mostRecentInstanceInverseIdx;
				int& mostRecentlyAddedInstanceIdx = cache.sections[mostRecentInstanceInverseIdx.x].instanceIndices[mostRecentInstanceInverseIdx.y];
				//check(mostRecentlyAddedInstanceIdx != instanceIdx);
				mostRecentlyAddedInstanceIdx = instanceIdx;
				InstancedMesh->RemoveInstance(instanceIdx);
			}
			cache.inverseLookup.SetNum(cache.inverseLookup.Num()- chunk.instanceIndices.Num());
			chunk.instanceIndices.Empty();
			*/
			//check(validateCache());
			chunk.isPopulated = false;
		}
	}


};
