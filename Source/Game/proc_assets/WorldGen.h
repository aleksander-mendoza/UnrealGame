// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "../blender/proc_assets.h"
#include "WorldGenUtils.h"
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
	int resolutionX=32;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int resolutionYDistant = 4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int resolutionXDistant = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	float  chunkW=100;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	float chunkH=100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int renderRadius = 8; // chunks within this radius will be rendered but not necessarily generated if not present
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int detailedRenderRadius = 1; // chunks within this radius will be generated in high-res if not present (or present but in low-res)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int lowResRenderRadius = 3; // chunks within this radius will be generated (in low-res) if not present

	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	float seaLevel=-10;

	UPROPERTY(BlueprintReadOnly) 
	UProceduralMeshComponent* TerrainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	UMaterialInterface* TerrainMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float  rockDensity = 100;
	
	
	
private:
	APawn* PlayerPawn;
	int2 currentPlayerChunkPos;
	int sectionCount = 0;
	bool isBusy;
	TArray<MeshGenResult> resultStack;
	TMap<int2, int> generatedChunks;
	TQueue<MeshGenRequest> requestQueue;
	
	inline const int * getSectionIdx(int2 chunkPos) {
		return generatedChunks.Find(chunkPos);
	}
	inline const bool hasChunk(int2 chunkPos) {
		return generatedChunks.Contains(chunkPos);
	}
	inline int setSectionIdx(int2 chunkPos, int sectionIdx) {
		check(!hasChunk(chunkPos));
		return generatedChunks.Add(chunkPos, sectionIdx);
	}
	inline bool removeSectionIdx(int2 chunkPos, int & sectionIdx) {
		return generatedChunks.RemoveAndCopyValue(chunkPos, sectionIdx);
	}
	inline const int2 getChunkPos(double posX, double posY) {
		return int2(floor(posX / this->chunkW), floor(posY / this->chunkH));
	}
	inline const int2 getChunkPos(FVector pos) {
		return getChunkPos(pos.X, pos.Y);
	}
	inline const int2 getPlayerChunkPos() {
		if (PlayerPawn == nullptr)return int2(0, 0);
		FVector playerPosition = PlayerPawn->GetActorLocation();
		int2 chunkPos = getChunkPos(playerPosition);
		return chunkPos;
	}

	void popGenResult();
	inline void addMeshSection(MeshGenResult& mesh) {
		addMeshSection(mesh.sectionIdx, mesh.mesh);
	}
	inline void addMeshSection(int sectionIdx, proc_assets::Mesh & mesh){
		this->TerrainMesh->CreateMeshSection_LinearColor(sectionIdx, mesh.vertices, mesh.triangles, mesh.normals, mesh.uvs, TArray<FLinearColor>(), mesh.tangents, true);
		if (this->TerrainMaterial != nullptr) this->TerrainMesh->SetMaterial(sectionIdx, this->TerrainMaterial);
	}
	inline void updateMeshSection(MeshGenResult& mesh) {
		updateMeshSection(mesh.sectionIdx, mesh.mesh);
	}
	inline void updateMeshSection(int sectionIdx, proc_assets::Mesh& mesh) {
		this->TerrainMesh->UpdateMeshSection_LinearColor(sectionIdx, mesh.vertices, mesh.normals, mesh.uvs, TArray<FLinearColor>(), mesh.tangents);
	}
	inline void removeMeshSection(int sectionIdx){
		this->TerrainMesh->ClearMeshSection(sectionIdx);
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void requestAddOrUpdateChunksInRadius(int2 centerPos, int radius, int resX, int resY, TArray<ChunkDist>& existingChunks);
	inline void requestAddOrUpdateChunksAroundPlayer(TArray<ChunkDist>& existingChunks) {
		requestAddOrUpdateChunksInRadius(currentPlayerChunkPos, detailedRenderRadius, resolutionX, resolutionY, existingChunks);
	}
	inline void generateAndAddChunk(const int2 chunkPos, int resX, int resY) {
		check(!hasChunk(chunkPos));
		generateAndAddChunk(chunkPos, resX, resY, sectionCount++, true);
	}
	inline void generateAndAddChunk(const int2 chunkPos, int resX, int resY, int sectionIdx, bool createNew) {
		MeshGenResult m;
		m.createNew = createNew;
		setSectionIdx(chunkPos, sectionIdx);
		generateChunk(chunkPos, resX, resY, sectionIdx, m, createNew);
		if (createNew)addMeshSection(m);
		else updateMeshSection(m);
	}
	void requestAddOrUpdateChunk(const int2 chunkPos, int resX, int resY, TArray<ChunkDist>& existingChunks);
	inline void generateAndAddChunk(const MeshGenRequest& r) {
		generateAndAddChunk(r.chunkPos, r.resX, r.resY, r.sectionIdx, r.createNew);
	}
	inline void generateChunk(const MeshGenRequest &r, MeshGenResult& s) {
		s.createNew = r.createNew;
		generateChunk(r.chunkPos,r.resX,r.resY,r.sectionIdx,s,r.createNew);
	}
	inline void generateChunk(const int2 chunkPos, int resX, int resY, int sectionIdx, MeshGenResult& s, bool genTriangles) {
		const FVector offset = FVector(chunkPos.X * this->chunkW, chunkPos.Y * this->chunkH, this->seaLevel);
		const float2 size = float2(this->chunkW, this->chunkH);
		check(s.createNew == genTriangles);
		s.sectionIdx = sectionIdx;
		//proc_assets::morenoise(offset, resX, resY, size, mesh, scale, pointiness, scalingPowerBase, numOfScales, maxHeight);
		proc_assets::perlin_fbm(offset, resX, resY, size, s.mesh, scale, scalingPowerBase, 1. / scalingPowerBase, numOfScales, maxHeight, genTriangles);
	}
	inline void requestChunkIfNotExists(const int2 chunkPos, int resX, int resY){
		if (getSectionIdx(chunkPos) ==nullptr) {
			requestQueue.Enqueue(MeshGenRequest{ .chunkPos = chunkPos, .resX=resX,.resY=resY,.sectionIdx= sectionCount,.createNew=true});
			setSectionIdx(chunkPos, sectionCount++);
		}
	}
	inline void requestChunkOrUpdate(const int2 chunkPos, int resX, int resY) {
		const int* sectionIdx = getSectionIdx(chunkPos);
		if (sectionIdx) {
			requestQueue.Enqueue(MeshGenRequest{ .chunkPos= chunkPos, .resX = resX,.resY = resY,.sectionIdx = *sectionIdx,.createNew = false});
		}else{
			requestQueue.Enqueue(MeshGenRequest{ .chunkPos= chunkPos, .resX = resX,.resY = resY,.sectionIdx = sectionCount,.createNew = true });
			setSectionIdx(chunkPos, sectionCount++);
		}
	}
	void dequeueRequestAsync();
	void dequeueRequestSync() {
		MeshGenRequest i;
		if (requestQueue.Dequeue(i)) {
			generateAndAddChunk(i);
		}
	}

};
