// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"
#include "KismetProceduralMeshLibrary.h"
#include "../blender/proc_assets.h"
#include "../blender/rand.h"
#include "../blender/noise.h"
#include "Logging/StructuredLog.h"
#include "../proc_assets/Rock.h"

void AWorldGen::shiftSurroundingChunks(int2 shift) {
	
	int diameter = getRenderDiameter();
	check(diameter >= 3);
	check(shift.X == math::sgn(shift.X));
	check(shift.Y == math::sgn(shift.Y));
	int offset = shift.X + shift.Y * diameter;
	int dX=1, dY=1, sX=0, sY=0, eX= diameter, eY= diameter;
	if (shift.X != 0) {
		if (shift.X > 0) {
			dX = -1;
			sX = diameter - 1;
			eX = 0;
		}
		else {
			sX = 0;
			eX = diameter - 1;
		}
		
		for (int y = 0; y < diameter; y++) {
			dropChunk(sX + y * diameter);
		}
	}
	if (shift.Y != 0) {
		if (shift.Y > 0) {
			dY = -1;
			sY = diameter - 1;
			eY = 0;
		}
		else {
			sY = 0;
			eY = diameter - 1;
		}

		for (int x = 0; x < diameter; x++) {
			dropChunk(x + sY * diameter);
		}
	}
	
	for (int x = sX; x != eX; x += dX) {
		for (int y = sY; y != eY; y += dY) {
			const int toIdx = x + y * diameter;
			const int fromIdx = toIdx - offset;
			const int2 fromPos = getChunkRelPos(fromIdx);
			const int2 toPos = getChunkRelPos(toIdx);
			check(fromPos + int2(shift.X, shift.Y) == toPos);
			surroundingChunks[toIdx] = surroundingChunks[fromIdx];
		}
	}
	if (shift.X != 0) {
		for (int y = 0; y < diameter; y++) {
			surroundingChunks[eX + y * diameter]=-1;
		}
	}
	if (shift.Y != 0) {
		for (int x = 0; x < diameter; x++) {
			surroundingChunks[x + eY * diameter] = -1;
		}
	}
	
}
const FString AWorldGen::toDebugStr()
{
	FString s;
	const int d = getRenderDiameter();
	for (int y = d-1; y >= 0; y--) {
		for (int x = 0; x < d; x++) {
			s += FString::Printf(TEXT("%3d"), surroundingChunks[getChunkIdxFromRelPos(int2(x,y))]);
			s += ' ';
		}
		s += '\n';
	}
	return s;
}
// Sets default values
AWorldGen::AWorldGen() : unusedSectionIndices(), meshGenResult(), surroundingChunks(), requestQueue()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	TerrainMesh->SetupAttachment(GetRootComponent());
	TerrainMesh->bUseAsyncCooking = true;

	

}
void AWorldGen::PostInitializeComponents()
{
	for (FFoliageParams params: FoliageParams) {
		UInstancedStaticMeshComponent * foliageMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FoliageMesh"));
		FoliageMeshes.Add(foliageMesh);
		foliageMesh->SetStaticMesh(params.Mesh);
		TerrainMesh->SetupAttachment(foliageMesh);
	}
	
}
void AWorldGen::resetSurroundingChunks() {
	this->TerrainMesh->ClearAllMeshSections();
	surroundingChunks.Empty();
	unusedSectionIndices.Empty();
	int renderArea = this->getRenderArea();
	surroundingChunks.Reserve(renderArea);
	unusedSectionIndices.Reserve(renderArea);
	while (surroundingChunks.Num() < renderArea) {
		unusedSectionIndices.Add(surroundingChunks.Num());
		surroundingChunks.Add(-1);
		check(unusedSectionIndices.Last() >= 0);
	}
	check(unusedSectionIndices.Num() == surroundingChunks.Num());
	check(unusedSectionIndices.Num() == renderArea);
}
// Called when the game starts or when spawned
void AWorldGen::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = this->GetWorld();
	APlayerController* player = world->GetFirstPlayerController();
	PlayerPawn = player->GetPawn();
	resetCenter();
	resetSurroundingChunks();
	generateChunksInRadius(absChunkOffset, detailedRenderRadius, resolutionX, resolutionY, true);
	check(requestQueue.IsEmpty());
	check(!isMeshGenResultReady);
	check(!isBusy);
}

void AWorldGen::distributeFoliage(const int2 chunkAbsPos, const int foliageIdx)
{
	const FFoliageParams& params = FoliageParams[foliageIdx];
	const UInstancedStaticMeshComponent* instances = FoliageMeshes[foliageIdx];
	const FVector offset = FVector(chunkAbsPos.X * this->chunkW, chunkAbsPos.Y * this->chunkH, this->seaLevel);
	const int count = int(params.density * chunkW * chunkH);
	const int seed = noise::hash(chunkAbsPos.X, chunkAbsPos.Y);
	blender::RandomNumberGenerator rng(seed);
	for (int i = 0; i < count; i++) {
		if (params.alignToNormal) {
			//const float2 position = offsetf + size * rng.get_float2();
			//const float3 gradient_and_height = noise::morenoise(position, scale, pointiness, scalingPowerBase, numOfScales) * maxHeight;
			//const float3 gradient_and_height = noise::perlin_noise_derivative(position, scale) * maxHeight;

		}
		else {

		}
	}

}

void AWorldGen::generateChunksInRadius(int2 centerPos, int radius, int resX, int resY, bool dontOverwrite) {
	generateAndAddChunk(centerPos, resX, resY, dontOverwrite);
	for (int dist = 1; dist <= radius; dist++) {
		for (int x = centerPos.X - dist; x <= centerPos.X + dist; x++) {
			generateAndAddChunk(int2(x, centerPos.Y + dist), resX, resY, dontOverwrite);
			generateAndAddChunk(int2(x, centerPos.Y - dist), resX, resY, dontOverwrite);
		}
		for (int y = centerPos.Y - dist + 1; y < centerPos.Y + dist; y++) {
			generateAndAddChunk(int2(centerPos.X - dist, y), resX, resY, dontOverwrite);
			generateAndAddChunk(int2(centerPos.X + dist, y), resX, resY, dontOverwrite);
		}
	}
}


void AWorldGen::dequeueRequestAsync()
{
	check(!isBusy);
	MeshGenRequest i;
	if (requestQueue.Dequeue(i)) {
		isBusy = true;
		check(getRenderArea() > i.sectionIdx);
		check(i.sectionIdx >= 0);
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [i, this]()
			{
				MeshGenRequest j = i;
				generateAndPushChunk(j);
				check(meshGenResult.sectionIdx == i.sectionIdx);
				isMeshGenResultReady = true;
			}
		);
	}
	
}

// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);
	int2 playerPos = getPlayerAbsChunk();
	if (playerPos != absChunkOffset) {
		const int2 shift = absChunkOffset - playerPos;
		shiftSurroundingChunks(shift);
		int yStart = 0, yEnd = -1, xOffset = 0;
		if (playerPos.X != absChunkOffset.X) {
			yStart = renderRadius - detailedRenderRadius;
			yEnd = renderRadius + detailedRenderRadius;
			xOffset = renderRadius - shift.X * detailedRenderRadius;
		}
		int xStart = 0, xEnd = -1, yOffset = 0;
		if (playerPos.Y != absChunkOffset.Y) {
			xStart = renderRadius - detailedRenderRadius;
			xEnd = renderRadius + detailedRenderRadius;
			yOffset = renderRadius - shift.Y * detailedRenderRadius;
		}
		absChunkOffset = playerPos;
		for (int x = xStart; x <= xEnd; x++) {
			requestChunk(getChunkAbsPosFromRelPos(int2(x, yOffset)), resolutionX, resolutionY, true);
		}
		for (int y = yStart; y <= yEnd; y++) {
			requestChunk(getChunkAbsPosFromRelPos(int2(xOffset, y)), resolutionX, resolutionY, true);
		}
		//UE_LOGFMT(LogCore, Warning, "Status=\n{0}", toDebugStr());
		
	}
	if (isBusy) {
		if (isMeshGenResultReady) {
			addMeshSection(meshGenResult);
			isMeshGenResultReady = false;
			isBusy = false;
		}
	}
	else {
		dequeueRequestAsync();
	}
	

}