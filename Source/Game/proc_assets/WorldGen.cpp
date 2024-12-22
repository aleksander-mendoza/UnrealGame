// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"
#include "KismetProceduralMeshLibrary.h"
#include "../blender/proc_assets.h"
#include "../blender/rand.h"
#include "../blender/noise.h"
#include "Logging/StructuredLog.h"
#include "../proc_assets/Rock.h"


// Sets default values
AWorldGen::AWorldGen():currentPlayerChunkPos(0xFFFFFFFF, 0xFFFFFFFF)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	TerrainMesh->SetupAttachment(GetRootComponent());
	TerrainMesh->bUseAsyncCooking = true;
	
}
void AWorldGen::popGenResult()
{
	if (!resultStack.IsEmpty()) {
		MeshGenResult m = resultStack.Pop();		
		if (m.createNew) {
			addMeshSection(m);
		}
		else {
			updateMeshSection(m);
		}
	}
}
// Called when the game starts or when spawned
void AWorldGen::BeginPlay()
{
	Super::BeginPlay();
	UWorld * world = this->GetWorld();
	APlayerController* player = world->GetFirstPlayerController();
	PlayerPawn = player->GetPawn();
}

void AWorldGen::requestAddOrUpdateChunksInRadius(int2 centerPos, int radius, int resX, int resY, TArray<ChunkDist> & existingChunks) {
	UE_LOGFMT(LogCore, Warning, "Player={0},{1}", centerPos.X, centerPos.Y);
	requestAddOrUpdateChunk(centerPos, resX, resY, existingChunks);
	for (int dist = 1; dist <= radius; dist++) {
		for (int x = centerPos.X - dist; x <= centerPos.X + dist; x++) {
			requestAddOrUpdateChunk(int2(x, centerPos.Y + dist), resX, resY, existingChunks);
			requestAddOrUpdateChunk(int2(x, centerPos.Y - dist), resX, resY, existingChunks);
		}
		for (int y = centerPos.Y - dist + 1; y < centerPos.Y + dist; y++) {
			requestAddOrUpdateChunk(int2(centerPos.X - dist, y), resX, resY, existingChunks);
			requestAddOrUpdateChunk(int2(centerPos.X + dist, y), resX, resY, existingChunks);
		}
	}
}

void AWorldGen::requestAddOrUpdateChunk(const int2 chunkPos, int resX, int resY, TArray<ChunkDist>& existingChunks)
{
	if (hasChunk(chunkPos))return;
	int sectionIdx;
	const bool createNew = existingChunks.IsEmpty();
	if (createNew) {
		sectionIdx = sectionCount++;
		UE_LOGFMT(LogCore, Warning, "create new {0},{1}", chunkPos.X, chunkPos.Y);
	}
	else {
		const ChunkDist mostDistantChunk = existingChunks.Pop();
		const bool success = removeSectionIdx(mostDistantChunk.chunkPos, sectionIdx);
		UE_LOGFMT(LogCore, Warning, "replace {0},{1} with {2}, {3}", chunkPos.X, chunkPos.Y, mostDistantChunk.chunkPos.X, mostDistantChunk.chunkPos.Y);
		check(success);
	}
	requestQueue.Enqueue(MeshGenRequest{ .chunkPos = chunkPos, .resX = resX,.resY = resY,.sectionIdx = sectionIdx,.createNew = createNew });
	setSectionIdx(chunkPos, sectionIdx);
}

	//proc_assets::morenoise(offset, resX, resY, size, mesh, scale, pointiness, scalingPowerBase, numOfScales, maxHeight);
	
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
	

void AWorldGen::dequeueRequestAsync()
{
	MeshGenRequest i;
	check(!isBusy);
	if (requestQueue.Dequeue(i)) {
		isBusy = true;
		check(hasChunk(i.chunkPos));
		check(*getSectionIdx(i.chunkPos) == i.sectionIdx);
		UE_LOGFMT(LogCore, Warning, "Generating chunk {0},{1} new={2}", i.chunkPos.X, i.chunkPos.Y, i.createNew);
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [i, this]()
			{
				resultStack.AddDefaulted();
				generateChunk(i, resultStack.Last());
				isBusy = false;
			}
		);
	}
}

// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);
	int2 playerPos = getPlayerChunkPos();
	if (playerPos != currentPlayerChunkPos) {
		currentPlayerChunkPos = playerPos;
		
		TArray<ChunkDist> distances;
		distances.Reserve(generatedChunks.Num());
		for (TPair<int2, int> chunk : generatedChunks) {
			const int dX = math::abs(chunk.Key.X - playerPos.X);
			const int dY = math::abs(chunk.Key.Y - playerPos.Y);
			if (dX > renderRadius || dY > renderRadius) {
				distances.Add(ChunkDist{ .dist = dX + dY, .chunkPos = chunk.Key });
			}
		}

		distances.Sort([](const ChunkDist& a, const ChunkDist& b) {return a.dist < b.dist; });
		requestAddOrUpdateChunksInRadius(playerPos,detailedRenderRadius,resolutionX,resolutionY,distances);
	}
	if (!isBusy) {
		popGenResult();
		dequeueRequestAsync();
	}

}
