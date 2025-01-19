// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "ProceduralMeshComponent.h"
#include "../blender/proc_assets.h"
#include "EntityParams.h"
#include "WorldGenUtils.h"
#include "../items/ItemActor.h"
#include "../blender/noise.h"
#include "../GameCharacter.h"
#include "MovingGrid.h"
#include "../structures/BuildingSystem.h"
#include "WorldGen.generated.h"


UCLASS()
class GAME_API AWorldGen : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldGen();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool TerrainGenEnabled = true;
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
	float chunkSpawnRadius = 1; // chunks within this radius will be generated in high-res if not present (or present but in low-res)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int lowResRenderRadius = 3; // chunks within this radius will be generated (in low-res) if not present

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float seaLevel = -10;


	UPROPERTY(BlueprintReadOnly)
	UProceduralMeshComponent* TerrainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* TerrainMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEntityParams> EntityParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovingGrid TerrainGrid;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBuildingSystem BuildingSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class TSubclassOf<AGameCharacter> PlayerPawnClass;

	UPROPERTY()
	AGameCharacter* PlayerPawn;

	void PostInitializeComponents() override;
private:
	
	
	proc_assets::Mesh meshGenResult;
	int resultSectionIdx = -1;
	GenStatus genStatus=GenStatus::IDLE;
	TArray<int> entitySectionsToUnload;
	TArray<int> entitySectionsToLoad;
	int resultEntityIdx=-1;
	int resultEntityTypeIdx = -1;
	bool couldHaveMoreChunksToLoad = false;
	bool playerCrossedChunkBoundary = false;
	bool isBusy = false;
	

public:
	

	inline void reset() {
		const double3 pos = getPlayerPos();
		this->TerrainMesh->ClearAllMeshSections();
		TerrainGrid.reset(pos);
		for (FEntityParams& params : EntityParams) {
			params.reset(pos);
		}

	}
	inline const double3 getPlayerPos() {
		if (PlayerPawn == nullptr)return double3(0, 0, 0);
		return PlayerPawn->GetActorLocation();
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
	inline void generateEntity(int2 chunkAbsPos, const int chunkDist, const int entityIdx, const int sectionIdx) {
		FEntityParams& params = EntityParams[entityIdx];
		const float chunkSize = params.SpawnGrid.chunkSize;
		const float2 offsetf = float2(chunkAbsPos.X * chunkSize, chunkAbsPos.Y * chunkSize);
		for (int typeIdx = 0; typeIdx < params.Types.Num(); typeIdx++) {
			FEntityType& t = params.Types[typeIdx];
			EntityChunk& chunk = t.clearSection(sectionIdx);
			const int seed = noise::hash(chunkAbsPos.X, chunkAbsPos.Y, t.seed);
			blender::RandomNumberGenerator rng(seed);
			const int count = t.treatDensityAsMaxCount ? (rng.get_int32() % int(t.density+1)) : int(t.density * chunkSize * chunkSize);

			for (int i = 0; i < count; i++) {
				const float2 position = offsetf + chunkSize * rng.get_float2();
				float3 position3d = float3(position.X, position.Y, seaLevel);
				FRotator3d rot;
				if (t.alignToNormal) {
					const float3 gradient_and_height = get_height_with_derivative(position);
					const double3 steepestDescent = double3(gradient_and_height.X, gradient_and_height.Y, gradient_and_height.X* gradient_and_height.X + gradient_and_height.Y* gradient_and_height.Y);
					rot = steepestDescent.Rotation();
					//rot.Pitch = atanf(gradient_and_height.X)*180/PI;
					//rot.Yaw = atan2f(gradient_and_height.X, gradient_and_height.Y) * 180 / PI;
					position3d.Z += gradient_and_height.Z;
				}
				else {
					rot = FRotator3d();
					position3d.Z += get_height(position);
				}
				if (t.maxRotation > 0) {
					rot.Yaw += rng.get_float() * t.maxRotation;
				}
				chunk.instanceTransforms.Add(FTransform(rot, double3(position3d)));
			}
		}
	}
	inline void generateChunk(int2 chunkAbsPos, const int chunkDist, proc_assets::Mesh& mesh, const int sectionIdx) {
		const FVector offset = FVector(chunkAbsPos.X * TerrainGrid.chunkSize, chunkAbsPos.Y * TerrainGrid.chunkSize, this->seaLevel);
		
		const float2 size = float2(TerrainGrid.chunkSize, TerrainGrid.chunkSize);
		//proc_assets::morenoise(offset, resX, resY, size, mesh, scale, pointiness, scalingPowerBase, numOfScales, maxHeight);
		//proc_assets::perlin_fbm(offset, r.resX, r.resY, size, s.mesh, scale, scalingPowerBase, 1. / scalingPowerBase, numOfScales, maxHeight, uvScale, true);
		proc_assets::planeScaledUVs(offset, resolution, resolution, size, mesh, [&](FVector& vertex) {return get_height_with_derivative(float2(double2(vertex))); }, uvScale, true);
		mesh.hasTriangles = true;
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
		BuildingSystem.addChunksAroundPlayer<l2>();
		for (int i = 0; i < EntityParams.Num();i++) {
			EntityParams[i].addChunksAroundPlayer<l2>();
		}
	}
	inline void update() {
		const double3 playerPos = getPlayerPos();
		playerCrossedChunkBoundary = false;
		if (TerrainGrid.update(playerPos, chunkSpawnRadius)) {
			
		}
		if (BuildingSystem.update(playerPos)) {

		}
		playerCrossedChunkBoundary |= TerrainGrid.playerCrossedChunkBoundary;
		for (int i = 0; i < EntityParams.Num(); i++) {
			if (EntityParams[i].update(playerPos)) {
				UE_LOGFMT(LogCore, Warning, "Entity[{0}]=\n{1}",i, EntityParams[i].SpawnGrid.toDebugStr());
			}
			playerCrossedChunkBoundary |= EntityParams[i].SpawnGrid.playerCrossedChunkBoundary;
		}
	}
	template<bool async>
	inline bool generateChunksAroundPlayer() {
		if (!async || TerrainGrid.playerCrossedChunkBoundary || TerrainGrid.couldHaveMoreChunksToLoad) {
			if (async)TerrainGrid.playerCrossedChunkBoundary = false;
			for (int i = 0; i < TerrainGrid.chunkDistances.Num(); i++) {
				const ChunkDist& chunk = TerrainGrid.chunkDistances[i];
				const int sectionIdx = TerrainGrid.getSectionIdx(chunk.chunkIdx);
				const int2 absPos = TerrainGrid.absChunkOffset + chunk.chunkPos;  //TODO: here is a race condition. absChunkOffset could change since the last time we read sectionIdx.
				if (sectionIdx >= 0) {
					SectionStatus& sectionStatus = TerrainGrid.sections[sectionIdx];
					if (sectionStatus.newlyAdded) {						
						UE_LOGFMT(LogCore, Warning, "Request {0} at {1},{2}", sectionIdx, chunk.chunkPos.X, chunk.chunkPos.Y);
						if (async) {
							meshGenResult.clearEverythingButTriangles();
							generateChunk(absPos, chunk.distLinf, meshGenResult, sectionIdx);
							resultSectionIdx = sectionIdx;
							genStatus = GenStatus::MESH_GEN_RESULT_IS_READY;
							TerrainGrid.couldHaveMoreChunksToLoad = true;
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
			if (async)TerrainGrid.couldHaveMoreChunksToLoad = false;
		}
		check(entitySectionsToLoad.IsEmpty() && entitySectionsToUnload.IsEmpty());
		for (int entityIdx = 0; entityIdx < EntityParams.Num(); entityIdx++) {
			FEntityParams& params = EntityParams[entityIdx];
			FMovingGrid & grid = params.SpawnGrid;
			if (!async || grid.playerCrossedChunkBoundary || grid.couldHaveMoreChunksToLoad) {
				if (async)grid.playerCrossedChunkBoundary = false;
				for (int typeIdx = 0; typeIdx < params.Types.Num(); typeIdx++) {
					FEntityType& t = params.Types[typeIdx];
					EntityChunks & cache = t.chunked.cache;
					for (int i = 0; i < grid.chunkDistances.Num(); i++) {
						const ChunkDist& chunk = grid.chunkDistances[i];
						const int sectionIdx = grid.chunkToSection[chunk.chunkIdx];
						const int2 absPos = grid.absChunkOffset + chunk.chunkPos; //TODO: here is a race condition. absChunkOffset could change since the last time we read sectionIdx.
						if (sectionIdx > -1) {
							const float dist = chunk.distL2;
							SectionStatus& sectionStatus = grid.sections[sectionIdx];
							if (sectionStatus.newlyAdded) {
								sectionStatus.hasContent = true;
								sectionStatus.newlyAdded = false;
								sectionStatus.wentOutOfBounds = false;
								UE_LOGFMT(LogCore, Warning, "Generate entity {0} in section {1} at {2},{3}", entityIdx, sectionIdx, chunk.chunkPos.X, chunk.chunkPos.Y);
								generateEntity(absPos, dist, entityIdx, sectionIdx); // this does not require any sync with game thread 
								// so we don't return here like we did after generating chunk mesh
							}

							if (sectionStatus.wentOutOfBounds || t.unloadRadius <= dist) {
								if (cache.sections[sectionIdx].isLoaded) { 
									if (async) {
										UE_LOGFMT(LogCore, Warning, "Unload entity {0} in section {1} at {2},{3}", entityIdx, sectionIdx, chunk.chunkPos.X, chunk.chunkPos.Y);
										check(!entitySectionsToUnload.Contains(sectionIdx));
										entitySectionsToUnload.Add(sectionIdx);
									}
									else {
										t.unloadSection(sectionIdx);
									}
								}
								else {
									check(!cache.sections[sectionIdx].isDirty);
								}

							}else if (t.loadRadius >= dist) {
								if (!cache.sections[sectionIdx].isLoaded || cache.sections[sectionIdx].isDirty) {
									if (async) {
										UE_LOGFMT(LogCore, Warning, "Load entity {0} in section {1} at {2},{3}", entityIdx, sectionIdx, chunk.chunkPos.X, chunk.chunkPos.Y);
										check(!entitySectionsToLoad.Contains(sectionIdx));
										entitySectionsToLoad.Add(sectionIdx);
									}
									else {
										t.loadSection(sectionIdx);
									}
								}
							}

						}
					}
					if (async) {
						if (!entitySectionsToLoad.IsEmpty() || !entitySectionsToUnload.IsEmpty()) {
							resultEntityIdx = entityIdx;
							resultEntityTypeIdx = typeIdx;
							genStatus = GenStatus::FOLIAGE_UPDATE_IS_NEEDED;
							grid.couldHaveMoreChunksToLoad = true;
							return true;
						}
						
					}
				}
				grid.couldHaveMoreChunksToLoad = false;
			}
		}
		return false;
	}
	inline bool applyResults() {
		switch (genStatus) {
		case GenStatus::FOLIAGE_UPDATE_IS_NEEDED: {
			FEntityParams& params = EntityParams[resultEntityIdx];
			if (entitySectionsToLoad.IsEmpty()) {
				if (entitySectionsToUnload.IsEmpty()) {  
					genStatus = GenStatus::IDLE; // we finished popping everything
					return true;
				}
				else {
					//check(params.cache.sections[entitySectionsToUnload.Last()].isLoaded);
					params.Types[resultEntityTypeIdx].unloadSection(entitySectionsToUnload.Pop());
				}
			}
			else {
				if (entitySectionsToUnload.IsEmpty()) {
					//check(!params.cache.sections[entitySectionsToLoad.Last()].isLoaded);
					params.Types[resultEntityTypeIdx].loadSection(entitySectionsToLoad.Pop());
				}
				else {
					params.Types[resultEntityTypeIdx].loadUnloadSection(entitySectionsToLoad.Pop(), entitySectionsToUnload.Pop());
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
	// Called every Entity
	virtual void Tick(float DeltaTime) override;


};