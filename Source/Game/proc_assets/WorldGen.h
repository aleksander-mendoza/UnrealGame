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
	int resolutionY = 32;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int resolutionX = 32;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int chunkSpawnRadius = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double uvScale = 1;

	
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
	MeshGenResult meshGenResult;
	TArray<FTransform> foliageGenResult;
	bool genResultReady;
	int genResultSectionIdx;
	int genResultFoliageIdx;
	int genResultFoliageTypeIdx;
	bool isBusy = false;

	inline const int2 getAbsOffsetToBottomLeftmostChunk() {
		return int2(this->absChunkOffset.X - TerrainGrid.getRadius(), this->absChunkOffset.Y - TerrainGrid.getRadius());
	}
	inline const int2 absToRelPos(int2 chunkAbsPos) {
		return chunkAbsPos - getAbsOffsetToBottomLeftmostChunk();
	}
	inline const int2 relToAbsPos(int2 chunkRelPos) {
		return chunkRelPos + getAbsOffsetToBottomLeftmostChunk();
	}

	//void shiftSurroundingChunksUp();
	//void shiftSurroundingChunksDown();
	//void shiftSurroundingChunksLeft();
	//void shiftSurroundingChunksRight();
	
	inline const int2 getPlayerAbsChunk() {
		if (PlayerPawn == nullptr)return int2(0, 0);
		const FVector playerPosition = PlayerPawn->GetActorLocation();
		const int2 chunkPos = TerrainGrid.getChunkAbsPosFromWorldPos(playerPosition);
		return chunkPos;
	}
	inline void resetCenter() {
		absChunkOffset = getPlayerAbsChunk();
	}
	inline void addMeshSection(proc_assets::Mesh & mesh, int sectionIdx) {
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
	inline void generateChunk(int2 chunkAbsPos, int resX, int resY, proc_assets::Mesh & mesh) {
		const FVector offset = FVector(chunkAbsPos.X * TerrainGrid.chunkSize, chunkAbsPos.Y * TerrainGrid.chunkSize, this->seaLevel);
		const float2 size = float2(TerrainGrid.chunkSize, TerrainGrid.chunkSize);
		//proc_assets::morenoise(offset, resX, resY, size, mesh, scale, pointiness, scalingPowerBase, numOfScales, maxHeight);
		//proc_assets::perlin_fbm(offset, r.resX, r.resY, size, s.mesh, scale, scalingPowerBase, 1. / scalingPowerBase, numOfScales, maxHeight, uvScale, true);
		proc_assets::planeScaledUVs(offset, resX, resY, size, mesh, [&] (FVector& vertex) {return get_height_with_derivative(float2(double2(vertex))); }, uvScale, true);
		mesh.hasTriangles = true;
	}

	inline float3 get_height_with_derivative(float2 position) const {
		return noise::perlin_fbm_derivative(position, scale, maxHeight, scalingPowerBase, 1. / scalingPowerBase, numOfScales);
	}
	inline float get_height(float2 position) const {
		return noise::perlin_fbm(position, scale, maxHeight, scalingPowerBase, 1. / scalingPowerBase, numOfScales);
	}
	bool distributeFoliage(const int2 chunkAbsPos, const int sectionIdx, FFoliageParams& params);

	inline void reset() {
		this->TerrainMesh->ClearAllMeshSections();
		TerrainGrid.resetGridSize();
		for (FFoliageParams& params : FoliageParams) {
			params.reset();
		}

	}
	template<bool async>
	inline bool generateChunksAroundPlayer() {
		const int2 centerPos = absChunkOffset;
		for (int i = 0; i < chunkDistances.Num(); i++) {
			const ChunkDist & chunk = chunkDistances[i];
			int sectionIdx = getSectionIdx(chunk.chunkIdx);
			const int2 absPos = centerPos + chunkDistances[i].chunkPos;
			if (sectionIdx < 0) {
				sectionIdx = unusedSectionIndices.Pop();
				//const int2 relPos = getChunkRelPosFromAbsPos(chunkAbsPos);
				//UE_LOGFMT(LogCore, Warning, "Request {0} at {1},{2}", r.sectionIdx, relPos.X, relPos.Y);
				this->surroundingChunks[chunk.chunkIdx] = sectionIdx;
				const int resX = resolutionX;
				const int resY = resolutionX;
				
				if (async) {
					proc_assets::Mesh* mesh = new proc_assets::Mesh();
					mesh->clearEverythingButTriangles();
					generateChunk(absPos, resX, resY, *mesh);
					AsyncTask(ENamedThreads::GameThread, [mesh, sectionIdx, this]() {
						addMeshSection(*mesh, sectionIdx);
						delete mesh;
					});
					return true;
				}
				else {
					proc_assets::Mesh mesh;
					generateChunk(absPos, resX, resY, mesh);
					addMeshSection(mesh,sectionIdx);
				}

			}
			for (int j = 0; j < FoliageParams.Num(); j++) {
				FFoliageParams& params = FoliageParams[j];
				if (params.spawnRadius >= chunk.distL1) {
					if (distributeFoliage(absPos, sectionIdx, params) && async)return true;
				}
				else if (params.spawnRadius + 1 < chunk.distL1) {
					params.clearSection(sectionIdx);
				}
			}
		}
		return false;
	}
	inline void addChunksAroundPlayer() {
		TerrainGrid.addChunksWithinRadius(chunkSpawnRadius);
		for (FFoliageParams& params : FoliageParams) {
			params.addChunksAroundPlayer();
		}
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};