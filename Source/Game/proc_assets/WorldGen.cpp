// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"
#include "KismetProceduralMeshLibrary.h"
#include "../blender/proc_assets.h"
#include "../blender/rand.h"
#include "../blender/noise.h"
#include "Logging/StructuredLog.h"
#include "../proc_assets/Rock.h"

// Sets default values
AWorldGen::AWorldGen() :  meshGenResult()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	TerrainMesh->SetupAttachment(GetRootComponent());
	TerrainMesh->bUseAsyncCooking = true;
}

void AWorldGen::PostInitializeComponents()
{ 
	Super::PostInitializeComponents();
	FoliageParams.Sort([](const FFoliageParams& ip1, const FFoliageParams& ip2) {
		return  ip1.spawnRadius > ip2.spawnRadius;
	});
	check(FoliageParams.Num() == 0 || FoliageParams[0].spawnRadius >= FoliageParams.Last().spawnRadius);
	for (int i = 0; i < FoliageParams.Num();i++) {
		FFoliageParams& params = FoliageParams[i];
		params.spawnRadius = math::min(params.spawnRadius, float(TerrainGrid.radius - 1));
		params.density = math::min(params.density, 0.01);
		if (params.seed == -1)params.seed = i;
		params.initMesh(this, TerrainMesh);
	}
	
}
// Called when the game starts or when spawned
void AWorldGen::BeginPlay()
{
	Super::BeginPlay();
	reset();
	addChunksAroundPlayer<false>();
	generateChunksAroundPlayer<false>();
	check(!isBusy);
}



// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);
	int2 playerPos = getPlayerAbsChunk();
	if (playerPos != absChunkOffset) {
		const int2 shift = absChunkOffset - playerPos;
		TerrainGrid.shiftSurroundingChunks(shift);
		absChunkOffset = playerPos;
		addChunksAroundPlayer<false>();
		playerCrossedChunkBoundary = true;
		UE_LOGFMT(LogCore, Warning, "Status=\n{0}", TerrainGrid.toDebugStr());
	}
	if (!isBusy) {
		if (applyResults() && (couldHaveMoreChunksToLoad || playerCrossedChunkBoundary)) {
			isBusy = true;
			AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
				{
					couldHaveMoreChunksToLoad = generateChunksAroundPlayer<true>();
					isBusy = false;
				}
			);
			playerCrossedChunkBoundary = false;
		}
	}
	

}