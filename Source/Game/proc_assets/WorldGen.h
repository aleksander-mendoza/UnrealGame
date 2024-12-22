// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "../blender/proc_assets.h"
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
	int2 absChunkOffset;
	TArray<int> unusedSectionIndices;
	bool isBusy;
	TArray<proc_assets::MeshSection> meshStack;
	TArray<int> surroundingChunks;
	TArray<int3> chunksToUpdateQueue;
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
	inline const void chunksSectionWentBeyondRenderBoundary(int chunkIdx, int sectionIdx) {
		this->surroundingChunks[chunkIdx] = -1;
		if (sectionIdx >= 0) {
			removeMeshSection(sectionIdx);
			unusedSectionIndices.Add(sectionIdx);
		}
	}
	
	void shiftSurroundingChunksUp();
	void shiftSurroundingChunksDown();
	void shiftSurroundingChunksLeft();
	void shiftSurroundingChunksRight();

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
	void popMeshStack();
	void addMeshSection(int sectionIdx, proc_assets::Mesh & mesh);
	inline void removeMeshSection(int sectionIdx){
		if(sectionIdx>=0)this->TerrainMesh->ClearMeshSection(sectionIdx);
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GenerateChunk(const int2 chunkRelPos, int sectionIdx);
	void GenerateChunkIfNotExists(const int2 chunkRelPos);
	void GenerateChunkAsync();
	void GenerateChunkSync();

};
