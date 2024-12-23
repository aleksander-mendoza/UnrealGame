// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "../blender/proc_assets.h"
#include "FoliageParams.h"
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
	int resolutionX = 32;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int resolutionYDistant = 4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int resolutionXDistant = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float  chunkW = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float chunkH = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double uvScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int renderRadius = 8; // chunks within this radius will be rendered but not necessarily generated if not present
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int detailedRenderRadius = 1; // chunks within this radius will be generated in high-res if not present (or present but in low-res)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int lowResRenderRadius = 3; // chunks within this radius will be generated (in low-res) if not present

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float seaLevel = -10;

	UPROPERTY(BlueprintReadOnly)
	TArray<UInstancedStaticMeshComponent*> FoliageMeshes;

	UPROPERTY(BlueprintReadOnly)
	UProceduralMeshComponent* TerrainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* TerrainMaterial = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FFoliageParams> FoliageParams;



private:
	APawn* PlayerPawn=nullptr;
	int2 absChunkOffset;
	TArray<int> unusedSectionIndices;
	bool isBusy = false;
	bool isMeshGenResultReady=false;
	MeshGenResult meshGenResult;
	TArray<int> surroundingChunks;
	TQueue<MeshGenRequest> requestQueue;
	inline const int getChunkIdxFromRelPos(int2 chunkRelPos) {
		int diameter = getRenderDiameter();
		if (chunkRelPos.Y < 0 || chunkRelPos.Y >= diameter || chunkRelPos.X < 0 || chunkRelPos.X >= diameter)return -1;
		int chunkIdx = chunkRelPos.X + diameter * chunkRelPos.Y;
		return chunkIdx;
	}
	inline const int getChunkIdxFromAbsPos(int2 chunkAbsPos) {
		return getChunkIdxFromRelPos(getChunkRelPosFromAbsPos(chunkAbsPos));
	}
	inline const int2 getAbsOffsetToBottomLeftmostChunk() {
		return int2(this->absChunkOffset.X - renderRadius, this->absChunkOffset.Y - renderRadius);
	}
	inline const int2 getChunkRelPosFromAbsPos(int2 chunkAbsPos) {
		return chunkAbsPos - getAbsOffsetToBottomLeftmostChunk();
	}
	inline const int2 getChunkAbsPos(int chunkIdx) {
		return getChunkAbsPosFromRelPos(getChunkRelPos(chunkIdx));
	}
	inline const int2 getChunkAbsPosFromRelPos(int2 chunkRelPos) {
		return chunkRelPos + getAbsOffsetToBottomLeftmostChunk();
	}
	inline const int2 getChunkAbsPosFromWorldPos(double posX, double posY) {
		return int2(floor(posX / this->chunkW), floor(posY / this->chunkH));
	}
	inline const int2 getChunkAbsPosFromWorldPos(FVector pos) {
		return getChunkAbsPosFromWorldPos(pos.X, pos.Y);
	}
	inline const int2 getChunkRelPos(int chunkIdx) {
		int diameter = getRenderDiameter();
		int chunkX = chunkIdx % diameter;
		int chunkY = chunkIdx / diameter;
		return int2(chunkX, chunkY);
	}

	inline const int getSectionIdx(int chunkIdx) {
		if (chunkIdx < 0) return -1;
		return this->surroundingChunks[chunkIdx];
	}
	inline const int getSectionIdx(int2 chunkRelPos) {
		return getSectionIdx(getChunkIdxFromRelPos(chunkRelPos));
	}
	inline const int setSectionIdx(int2 chunkRelPos, int sectionIdx) {
		return setSectionIdx(getChunkIdxFromRelPos(chunkRelPos), sectionIdx);
	}
	inline const int setSectionIdx(int chunkIdx, int sectionIdx) {
		if (chunkIdx == -1)return -1;
		int prev = this->surroundingChunks[chunkIdx];
		this->surroundingChunks[chunkIdx] = sectionIdx;
		return prev;
	}
	inline const void dropChunk(int chunkIdx) {
		int sectionIdx = this->surroundingChunks[chunkIdx];
		if (sectionIdx >= 0) {
			this->surroundingChunks[chunkIdx] = -1;
			removeMeshSection(sectionIdx);
			unusedSectionIndices.Add(sectionIdx);
		}
	}

	//void shiftSurroundingChunksUp();
	//void shiftSurroundingChunksDown();
	//void shiftSurroundingChunksLeft();
	//void shiftSurroundingChunksRight();
	void shiftSurroundingChunks(int2 shift);
	const FString toDebugStr();
	inline const int getRenderDiameter() {
		return 1 + this->renderRadius * 2;
	}
	inline const int getRenderArea() {
		int randerDiameter = this->getRenderDiameter();
		return randerDiameter * randerDiameter;
	}
	void resetSurroundingChunks();
	
	inline const int2 getPlayerAbsChunk() {
		if (PlayerPawn == nullptr)return int2(0, 0);
		FVector playerPosition = PlayerPawn->GetActorLocation();
		int2 chunkPos = getChunkAbsPosFromWorldPos(playerPosition);
		return chunkPos;
	}
	inline void resetCenter() {
		absChunkOffset = getPlayerAbsChunk();
	}
	inline void addMeshSection(MeshGenResult& r) {
		check(getRenderArea() > r.sectionIdx);
		check(r.sectionIdx >= 0);
		this->TerrainMesh->CreateMeshSection_LinearColor(r.sectionIdx, r.mesh.vertices, r.mesh.triangles, r.mesh.normals, r.mesh.uvs, TArray<FLinearColor>(), r.mesh.tangents, true);
		if (this->TerrainMaterial != nullptr) this->TerrainMesh->SetMaterial(r.sectionIdx, this->TerrainMaterial);
	}
	inline void updateMeshSection(MeshGenResult& r) {
		check(getRenderArea() > r.sectionIdx);
		check(r.sectionIdx >= 0);
		this->TerrainMesh->UpdateMeshSection_LinearColor(r.sectionIdx, r.mesh.vertices, r.mesh.normals, r.mesh.uvs, TArray<FLinearColor>(), r.mesh.tangents);
	}
	inline void removeMeshSection(int sectionIdx) {
		check(getRenderArea() > sectionIdx);
		check(sectionIdx >= 0);
		this->TerrainMesh->ClearMeshSection(sectionIdx);
	}
	inline void generateChunk(const MeshGenRequest& r, MeshGenResult& s) {
		const FVector offset = FVector(r.chunkAbsPos.X * this->chunkW, r.chunkAbsPos.Y * this->chunkH, this->seaLevel);
		const float2 size = float2(this->chunkW, this->chunkH);
		s.sectionIdx = r.sectionIdx;
		//proc_assets::morenoise(offset, resX, resY, size, mesh, scale, pointiness, scalingPowerBase, numOfScales, maxHeight);
		proc_assets::perlin_fbm(offset, r.resX, r.resY, size, s.mesh, scale, scalingPowerBase, 1. / scalingPowerBase, numOfScales, maxHeight, uvScale, true);
		s.mesh.hasTriangles = true;
	}
	inline MeshGenRequest makeChunkRequest(const int2 chunkAbsPos, const int resX, const int resY, bool dontOverwrite) {
		const int chunkIdx = getChunkIdxFromAbsPos(chunkAbsPos);
		check(chunkIdx >= 0);
		MeshGenRequest r{.chunkAbsPos=chunkAbsPos, .resX=resX, .resY=resY, .sectionIdx = getSectionIdx(chunkIdx) };
		if (r.sectionIdx < 0) {
			r.sectionIdx = unusedSectionIndices.Pop();
			//const int2 relPos = getChunkRelPosFromAbsPos(chunkAbsPos);
			//UE_LOGFMT(LogCore, Warning, "Request {0} at {1},{2}", r.sectionIdx, relPos.X, relPos.Y);
			setSectionIdx(chunkIdx, r.sectionIdx);
		}
		else if(dontOverwrite) {
			r.sectionIdx = -1;
		}
		return r;
	}
	inline void generateAndAddChunk(const int2 chunkAbsPos, const int resX, const int resY, bool dontOverwrite) {
		MeshGenRequest r = makeChunkRequest(chunkAbsPos, resX, resY, dontOverwrite);
		generateAndAddChunk(r);
	}
	inline void generateAndAddChunk(MeshGenRequest & r) {
		if (r.sectionIdx >= 0) {
			MeshGenResult s;
			generateChunk(r, s);
			addMeshSection(s);
		}
	}
	inline void generateAndPushChunk(const int2 chunkAbsPos, const int resX, const int resY, bool dontOverwrite) {
		MeshGenRequest r = makeChunkRequest(chunkAbsPos, resX, resY, dontOverwrite);
		generateAndPushChunk(r);
	}
	inline void generateAndPushChunk(MeshGenRequest& r) {
		if (r.sectionIdx >= 0) {
			meshGenResult.mesh.clearEverythingButTriangles();
			generateChunk(r, meshGenResult);
		}
	}
	void generateChunksInRadius(int2 centerPos, int radius, int resX, int resY, bool dontOverwrite);
	inline void requestChunk(const int2 chunkAbsPos, int resX, int resY, bool dontOverwrite) {
		requestChunk(makeChunkRequest(chunkAbsPos, resX, resY, dontOverwrite));
	}
	inline void requestChunk(MeshGenRequest r) {
		if(r.sectionIdx>=0)requestQueue.Enqueue(r);
	}
	void dequeueRequestAsync();
	void dequeueRequestSync() {
		MeshGenRequest i;
		if (requestQueue.Dequeue(i)) {
			generateAndAddChunk(i);
		}
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};