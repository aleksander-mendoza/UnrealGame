// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"
#include "KismetProceduralMeshLibrary.h"
#include "../blender/proc_assets.h"
#include "../blender/rand.h"
#include "../blender/noise.h"
#include "../proc_assets/Rock.h"


void AWorldGen::shiftSurroundingChunksUp() {
	int diameter = getRenderDiameter();
	for (int x = 0; x < diameter; x++) {
		int i = x;
		int prev = surroundingChunks[i];
		for (int y=0; y < diameter - 1; y++) {
			i += diameter;
			int next = surroundingChunks[i];
			surroundingChunks[i] = prev;
			prev = next;
		}
		chunksSectionWentBeyondRenderBoundary(x, prev);
	}
	
}
void AWorldGen::shiftSurroundingChunksDown() {
	int diameter = getRenderDiameter();
	int dd = diameter * (diameter - 1);
	for (int x = 0; x < diameter; x++) {
		int i = x + dd;
		int prev = surroundingChunks[i];
		for (int y = 0; y < diameter - 1; y++) {
			i -= diameter;
			int next = surroundingChunks[i];
			surroundingChunks[i] = prev;
			prev = next;
		}
		chunksSectionWentBeyondRenderBoundary(x, prev);
	}
}
void AWorldGen::shiftSurroundingChunksRight() {
	int diameter = getRenderDiameter();
	for (int i = 0, y = 0; y < diameter; y++) {
		int j = i;
		int prev = surroundingChunks[j];
		for (int x = 0; x < diameter-1; x++) {
			i++;
			int next = surroundingChunks[i];
			surroundingChunks[i] = prev;
			prev  = next;
		}
		chunksSectionWentBeyondRenderBoundary(j, prev);
		i++;
	}
}
void AWorldGen::shiftSurroundingChunksLeft() {
	int diameter = getRenderDiameter();
	for (int i = diameter*diameter-1, y = diameter-1; y >= 0; y--) {
		int j = i;
		int prev = surroundingChunks[j];
		for (int x = 0; x < diameter - 1; x++) {
			i--;
			int next = surroundingChunks[i];
			surroundingChunks[i] = prev;
			prev = next;
		}
		chunksSectionWentBeyondRenderBoundary(j, prev);
		i--;
	}
}
// Sets default values
AWorldGen::AWorldGen(): surroundingChunks()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	TerrainMesh->SetupAttachment(GetRootComponent());
	
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
		check(unusedSectionIndices.Last()>=0);
	}
	check(unusedSectionIndices.Num() == surroundingChunks.Num());
	check(unusedSectionIndices.Num() == renderArea);
}
// Called when the game starts or when spawned
void AWorldGen::BeginPlay()
{
	Super::BeginPlay();
	UWorld * world = this->GetWorld();
	APlayerController* player = world->GetFirstPlayerController();
	PlayerPawn = player->GetPawn();
	resetCenter();
	resetSurroundingChunks();

	this->GenerateChunk(int2(renderRadius, renderRadius), unusedSectionIndices.Pop());
	popMeshStack();
	for (int dist = 1; dist <= detailedRenderRadius; dist++) {
		for (int x = renderRadius-dist; x <= renderRadius + dist; x++) {
			this->GenerateChunk(int2(x, renderRadius + dist), unusedSectionIndices.Pop());
			popMeshStack();
			this->GenerateChunk(int2(x, renderRadius - dist), unusedSectionIndices.Pop());
			popMeshStack();
		}
		for (int y = renderRadius - dist + 1; y < renderRadius + dist; y++) {
			this->GenerateChunk(int2(renderRadius - dist, y), unusedSectionIndices.Pop());
			popMeshStack();
			this->GenerateChunk(int2(renderRadius + dist, y), unusedSectionIndices.Pop());
			popMeshStack();
		}
	}
	check(chunksToUpdateQueue.IsEmpty());
	check(meshStack.IsEmpty());
	check(!isBusy);
	
}
void AWorldGen::popMeshStack() {
	if (!meshStack.IsEmpty()) {
		proc_assets::MeshSection m = meshStack.Pop();
		addMeshSection(m.sectionIdx, m.mesh);
	}
}
void AWorldGen::addMeshSection(int sectionIdx, proc_assets::Mesh & mesh) {
	check(getRenderArea() > sectionIdx);
	check(sectionIdx > 0);
	this->TerrainMesh->CreateMeshSection_LinearColor(sectionIdx, mesh.vertices, mesh.triangles, mesh.normals, mesh.uvs, TArray<FLinearColor>(), mesh.tangents, true);
	if (this->TerrainMaterial != nullptr) this->TerrainMesh->SetMaterial(sectionIdx, this->TerrainMaterial);
}

void AWorldGen::GenerateChunkIfNotExists(const int2 chunkRelPos) {
	const int chunkIdx = getChunkIdxFromRelPos(chunkRelPos);
	int sectionIdx = surroundingChunks[chunkIdx];
	if (sectionIdx == -1) {
		surroundingChunks[chunkIdx] = sectionIdx = unusedSectionIndices.Pop();
		check(getRenderArea() > sectionIdx);
		check(sectionIdx >= 0);
		chunksToUpdateQueue.Add(int3(chunkRelPos.X, chunkRelPos.Y, sectionIdx));
	}

}
void AWorldGen::GenerateChunk(const int2 chunkRelPos, int sectionIdx) {
	check(getRenderArea() > sectionIdx);
	check(sectionIdx > 0);
	const int centerRelX = chunkRelPos.X - renderRadius;
	const int centerRelY = chunkRelPos.Y - renderRadius;
	const int x = absChunkOffset.X + centerRelX;
	const int y = absChunkOffset.Y + centerRelY;
	
	const float2 offsetf(x * this->chunkW, y * this->chunkH);
	const FVector offset = FVector(offsetf.X, offsetf.Y, this->seaLevel);
	const int resX = -detailedRenderRadius < centerRelX && centerRelX < detailedRenderRadius ? this->resolutionX : this->resolutionXDistant;
	const int resY = -detailedRenderRadius < centerRelY && centerRelY < detailedRenderRadius ? this->resolutionY : this->resolutionYDistant;

	const float2 size = float2(this->chunkW, this->chunkH);
	meshStack.AddDefaulted();
	proc_assets::MeshSection& s = meshStack.Last();
	s.sectionIdx = sectionIdx;
	//proc_assets::morenoise(offset, resX, resY, size, mesh, scale, pointiness, scalingPowerBase, numOfScales, maxHeight);
	proc_assets::perlin_fbm(offset, resX, resY, size, s.mesh, scale, scalingPowerBase, 1./scalingPowerBase,numOfScales, maxHeight);
	//proc_assets::perlin(offset, resX, resY, size, mesh, scale, maxHeight);
	/**
	blender::RandomNumberGenerator rng(noise::hash(x,y));
	for (int i = 0; i < 30; i++) {
		const float2 position = offsetf + size * rng.get_float2();
		//const float3 gradient_and_height = noise::morenoise(position, scale, pointiness, scalingPowerBase, numOfScales)* maxHeight;
		const float3 gradient_and_height = noise::perlin_fbm_derivative(position, scale, maxHeight, scalingPowerBase, 1./scalingPowerBase, numOfScales);
		//const float3 gradient_and_height = noise::perlin_noise_derivative(position, scale) * maxHeight;
	
		
		FActorSpawnParameters SpawnInfo;
		const double3 normal = math::normalize(math::normal(double2(float2(gradient_and_height))));
		const FRotator3d rot = normal.Rotation();
		const double3 position3d = double3(position.X, position.Y, this->seaLevel + gradient_and_height.Z);
		ARock* rock = GetWorld()->SpawnActor<ARock>(ARock::StaticClass(), position3d, rot, SpawnInfo);
		rock->SetActorLabel(FString::Printf(TEXT("ARock %d 0"), i));

		const double3 position3dX = position3d + double3(rockDensity,0, rockDensity* double(gradient_and_height.X));
		ARock* rock1 = GetWorld()->SpawnActor<ARock>(ARock::StaticClass(), position3dX, rot, SpawnInfo);
		rock1->SetActorLabel(FString::Printf(TEXT("ARock %d X"), i));

		const double3 position3dY = position3d + double3(0., rockDensity, rockDensity * double(gradient_and_height.Y));
		ARock* rock2 = GetWorld()->SpawnActor<ARock>(ARock::StaticClass(), position3dY, rot, SpawnInfo);
		rock2->SetActorLabel(FString::Printf(TEXT("ARock %d Y"), i));

		const double3 position3dZ = position3d + double(rockDensity)* normal;
		ARock* rock3 = GetWorld()->SpawnActor<ARock>(ARock::StaticClass(), position3dZ, rot, SpawnInfo);
		rock3->SetActorLabel(FString::Printf(TEXT("ARock %d Z"), i));
		
	}
	*/
	

}

void AWorldGen::GenerateChunkAsync()
{
	if (!isBusy && !chunksToUpdateQueue.IsEmpty()) {
		int3 i = chunksToUpdateQueue.Pop();
		isBusy = true;
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [i, this]()
			{
				const int2 chunkRelPos = int2(i.X, i.Y);
				const int sectionIdx = i.Z;
				GenerateChunk(chunkRelPos, sectionIdx);
				isBusy = false;
			}
		);
	}
}
void AWorldGen::GenerateChunkSync() {
	if (!chunksToUpdateQueue.IsEmpty()) {
		int3 i = chunksToUpdateQueue.Pop();
		const int2 chunkRelPos = int2(i.X, i.Y);
		const int sectionIdx = i.Z;
		GenerateChunk(chunkRelPos, sectionIdx);
		popMeshStack();
	}
}

// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);
	int2 playerPos = getPlayerAbsChunk();
	if (playerPos != absChunkOffset) {
		int yStart = 0, yEnd = -1, xOffset = 0;
		if (playerPos.X != absChunkOffset.X) {
			yStart = renderRadius - detailedRenderRadius;
			yEnd = renderRadius + detailedRenderRadius;
			if (playerPos.X < absChunkOffset.X) {
				xOffset = renderRadius - detailedRenderRadius;
				shiftSurroundingChunksRight();
			}
			else {
				xOffset = renderRadius + detailedRenderRadius;
				shiftSurroundingChunksLeft();
			}
		}
		
		int xStart = 0, xEnd = -1, yOffset = 0;
		if (playerPos.Y != absChunkOffset.Y) {
			xStart = renderRadius - detailedRenderRadius;
			xEnd = renderRadius + detailedRenderRadius;
			if (playerPos.Y < absChunkOffset.Y) {
				yOffset = renderRadius - detailedRenderRadius;
				shiftSurroundingChunksUp();
			}
			else {
				yOffset = renderRadius + detailedRenderRadius;
				shiftSurroundingChunksDown();
			}
		}
		absChunkOffset = playerPos;
		for (int x = xStart; x <= xEnd; x++) {
			GenerateChunkIfNotExists(int2(x, yOffset));
		}
		for (int y = yStart; y <= yEnd; y++) {
			GenerateChunkIfNotExists(int2(xOffset, y));
		}
	}
	GenerateChunkAsync();
	popMeshStack();


}
