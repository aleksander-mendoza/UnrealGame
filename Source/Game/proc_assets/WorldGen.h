// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "ProceduralMeshComponent.h"
#include "../blender/proc_assets.h"
#include "FoliageParams.h"
#include "WorldGenUtils.h"
#include "../blender/noise.h"
#include "MovingGrid.h"
#include "WorldGen.generated.h"


UCLASS()
class GAME_API AWorldGen : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldGen();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float pointiness = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxHeight = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float scale = 500;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int numOfScales = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float scalingPowerBase = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int resolution = 32;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double uvScale = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int chunkSpawnRadius = 1; // chunks within this radius will be generated in high-res if not present (or present but in low-res)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int lowResRenderRadius = 3; // chunks within this radius will be generated (in low-res) if not present

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float seaLevel = -10;


	UPROPERTY(BlueprintReadOnly)
	UProceduralMeshComponent* TerrainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* TerrainMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FFoliageParams> FoliageParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovingGrid TerrainGrid;


	void PostInitializeComponents() override;
private:
	APawn* PlayerPawn=nullptr;
	int2 absChunkOffset;
	proc_assets::Mesh meshGenResult;
	int resultSectionIdx = -1;
	GenStatus genStatus=GenStatus::IDLE;
	TArray<int> foliageSectionsToUnload;
	TArray<int> foliageSectionsToLoad;
	int resultFoliageIdx=-1;
	bool isBusy = false;
	bool couldHaveMoreChunksToLoad=false;
	bool playerCrossedChunkBoundary = false;
	
	

	inline const int2 getAbsOffsetToBottomLeftmostChunk() {
		return int2(this->absChunkOffset.X - TerrainGrid.getRadius(), this->absChunkOffset.Y - TerrainGrid.getRadius());
	}
	inline const int2 absToRelPos(int2 chunkAbsPos) {
		return chunkAbsPos - getAbsOffsetToBottomLeftmostChunk();
	}
	inline const int2 relToAbsPos(int2 chunkRelPos) {
		return chunkRelPos + getAbsOffsetToBottomLeftmostChunk();
	}

	inline void reset() {
		UWorld* world = this->GetWorld();
		APlayerController* player = world->GetFirstPlayerController();
		PlayerPawn = player->GetPawn();
		absChunkOffset = getPlayerAbsChunk();
		this->TerrainMesh->ClearAllMeshSections();
		TerrainGrid.resetGridSize();
		for (FFoliageParams& params : FoliageParams) {
			params.resetCache(TerrainGrid.getArea());
		}

	}
	inline const int2 getPlayerAbsChunk() {
		if (PlayerPawn == nullptr)return int2(0, 0);
		const FVector playerPosition = PlayerPawn->GetActorLocation();
		const int2 chunkPos = TerrainGrid.getChunkAbsPosFromWorldPos(playerPosition);
		return chunkPos;
	}
	inline void addMeshSection(proc_assets::Mesh& mesh, int sectionIdx) {
		check(sectionIdx >= 0);
		this->TerrainMesh->CreateMeshSection_LinearColor(sectionIdx, mesh.vertices, mesh.triangles, mesh.normals, mesh.uvs, TArray<FLinearColor>(), mesh.tangents, true);
		if (this->TerrainMaterial != nullptr) this->TerrainMesh->SetMaterial(sectionIdx, this->TerrainMaterial);
	}
	inline void updateMeshSection(proc_assets::Mesh& mesh, int sectionIdx) {
		check(sectionIdx >= 0);
		this->TerrainMesh->UpdateMeshSection_LinearColor(sectionIdx, mesh.vertices, mesh.normals, mesh.uvs, TArray<FLinearColor>(), mesh.tangents);
	}
	inline void removeMeshSection(int sectionIdx) {
		check(sectionIdx >= 0);
		this->TerrainMesh->ClearMeshSection(sectionIdx);
	}
	inline void generateChunk(int2 chunkAbsPos, const int chunkDist, proc_assets::Mesh& mesh, const int sectionIdx) {
		const FVector offset = FVector(chunkAbsPos.X * TerrainGrid.chunkSize, chunkAbsPos.Y * TerrainGrid.chunkSize, this->seaLevel);
		const float2 offsetf = float2(offset.X, offset.Y);
		const float2 size = float2(TerrainGrid.chunkSize, TerrainGrid.chunkSize);
		//proc_assets::morenoise(offset, resX, resY, size, mesh, scale, pointiness, scalingPowerBase, numOfScales, maxHeight);
		//proc_assets::perlin_fbm(offset, r.resX, r.resY, size, s.mesh, scale, scalingPowerBase, 1. / scalingPowerBase, numOfScales, maxHeight, uvScale, true);
		proc_assets::planeScaledUVs(offset, resolution, resolution, size, mesh, [&](FVector& vertex) {return get_height_with_derivative(float2(double2(vertex))); }, uvScale, true);
		mesh.hasTriangles = true;
		for (int foliageIdx = 0; foliageIdx < FoliageParams.Num(); foliageIdx++) {
			FFoliageParams& params = FoliageParams[foliageIdx];
			FoliageChunk& chunk = params.clearSection(sectionIdx);
			
			
			const int count = math::min(int(params.density * TerrainGrid.chunkSize * TerrainGrid.chunkSize), 1000);
			const int seed = noise::hash(chunkAbsPos.X, chunkAbsPos.Y);
			blender::RandomNumberGenerator rng(seed);

			for (int i = 0; i < count; i++) {
				const float2 position = offsetf + size * rng.get_float2();
				float3 position3d = float3(position.X, position.Y, seaLevel);
				FRotator3d rot;
				if (params.alignToNormal) {
					const float3 gradient_and_height = get_height_with_derivative(position);
					const double3 normal = math::normalize(math::normal(double2(float2(gradient_and_height))));
					rot = normal.Rotation();
					position3d.Z += gradient_and_height.Z;
				}
				else {
					position3d.Z += get_height(position);
				}
				if (params.maxRotation > 0) {
					rot.Roll = rng.get_float() * params.maxRotation;
				}
				chunk.instanceTransforms.Add(FTransform(rot, double3(position3d)));
			}
		}
	}

	inline float3 get_height_with_derivative(float2 position) const {
		return noise::perlin_fbm_derivative(position, scale, maxHeight, scalingPowerBase, 1. / scalingPowerBase, numOfScales);
	}
	inline float get_height(float2 position) const {
		return noise::perlin_fbm(position, scale, maxHeight, scalingPowerBase, 1. / scalingPowerBase, numOfScales);
	}
	template<bool l2>
	inline void addChunksAroundPlayer() {
		TerrainGrid.addChunksWithinRadius<l2>(chunkSpawnRadius);
	}
	template<bool async>
	inline bool generateChunksAroundPlayer() {
		const int2 centerPos = absChunkOffset;
		for (int i = 0; i < TerrainGrid.chunkDistances.Num(); i++) {
			const ChunkDist& chunk = TerrainGrid.chunkDistances[i];
			const int sectionIdx = TerrainGrid.getSectionIdx(chunk.chunkIdx);
			if (sectionIdx >= 0) {
				SectionStatus& sectionStatus = TerrainGrid.sections[sectionIdx];
				if (sectionStatus.newlyAdded) {
					const int2 absPos = centerPos + chunk.chunkPos;
					UE_LOGFMT(LogCore, Warning, "Request {0} at {1},{2}", sectionIdx, chunk.chunkPos.X, chunk.chunkPos.Y);

					if (async) {
						meshGenResult.clearEverythingButTriangles();
						generateChunk(absPos, chunk.distLinf, meshGenResult, sectionIdx);
						resultSectionIdx = sectionIdx;
						genStatus = GenStatus::MESH_GEN_RESULT_IS_READY;
						return true;
					}
					else {
						proc_assets::Mesh mesh;
						if (sectionStatus.wentOutOfBounds) {
							check(sectionStatus.hasContent);
							removeMeshSection(sectionIdx);
							sectionStatus.wentOutOfBounds = false;
							sectionStatus.hasContent = false;
						}
						generateChunk(absPos, chunk.distLinf, mesh, sectionIdx);
						addMeshSection(mesh, sectionIdx);
						sectionStatus.newlyAdded = false;
						sectionStatus.hasContent = true;
					}					
				}
			}
		}
		check(foliageSectionsToLoad.IsEmpty() && foliageSectionsToUnload.IsEmpty());
		for (int j = 0; j < FoliageParams.Num(); j++) {
			FFoliageParams& params = FoliageParams[j];
			for (int i = 0; i < TerrainGrid.chunkDistances.Num(); i++) {
				const ChunkDist& chunk = TerrainGrid.chunkDistances[i];
				const int sectionIdx = TerrainGrid.chunkToSection[chunk.chunkIdx];
				if (sectionIdx > -1) {
					if (params.spawnRadius >= chunk.distL2) {
						if (!params.cache.sections[sectionIdx].isLoaded) {
							UE_LOGFMT(LogCore, Warning, "Load {0} at {1},{2}", sectionIdx, chunk.chunkPos.X, chunk.chunkPos.Y);
							check(!foliageSectionsToLoad.Contains(sectionIdx));
							foliageSectionsToLoad.Add(sectionIdx);
						}
					}
					else if (params.spawnRadius + 1 < chunk.distL2) {
						if (params.cache.sections[sectionIdx].isLoaded) {
							UE_LOGFMT(LogCore, Warning, "Unload {0} at {1},{2}", sectionIdx, chunk.chunkPos.X, chunk.chunkPos.Y);
							check(!foliageSectionsToUnload.Contains(sectionIdx));
							foliageSectionsToUnload.Add(sectionIdx);
						}
					}
				}
			}
			if (!foliageSectionsToLoad.IsEmpty() || !foliageSectionsToUnload.IsEmpty()) {
				resultFoliageIdx = j;
				genStatus = GenStatus::FOLIAGE_UPDATE_IS_NEEDED;
				return true;
			}
		}
		return false;
	}
	inline bool applyResults() {
		switch (genStatus) {
		case GenStatus::FOLIAGE_UPDATE_IS_NEEDED: {
			FFoliageParams& params = FoliageParams[resultFoliageIdx];
			if (foliageSectionsToLoad.IsEmpty()) {
				if (foliageSectionsToUnload.IsEmpty()) {  
					genStatus = GenStatus::IDLE; // we finished popping everything
					return true;
				}
				else {
					check(params.cache.sections[foliageSectionsToUnload.Last()].isLoaded);
					params.unloadSection(foliageSectionsToUnload.Pop());
				}
			}
			else {
				if (foliageSectionsToUnload.IsEmpty()) {
					check(!params.cache.sections[foliageSectionsToLoad.Last()].isLoaded);
					params.loadSection(foliageSectionsToLoad.Pop());
				}
				else {
					params.loadUnloadSection(foliageSectionsToLoad.Pop(), foliageSectionsToUnload.Pop());
				}
			}
			return false;
		}
		case GenStatus::MESH_GEN_RESULT_IS_READY: {
			SectionStatus& sectionStatus = TerrainGrid.sections[resultSectionIdx];
			if (sectionStatus.hasContent) {
				removeMeshSection(resultSectionIdx);
				sectionStatus.hasContent = false;
			}
			if (sectionStatus.newlyAdded) { //it is possible (but not very likely) that the chunk came into and went out of view faster than we managed to generate it
				UE_LOGFMT(LogCore, Warning, "Added chunk {0}", resultSectionIdx);
				addMeshSection(meshGenResult, resultSectionIdx);
				sectionStatus.hasContent = true;
			}
			sectionStatus.newlyAdded = false;
			genStatus = GenStatus::IDLE;
			return true;
		}
		case GenStatus::IDLE:
			return true;
		}
		return true;
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};