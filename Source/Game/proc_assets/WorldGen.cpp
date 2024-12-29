// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"
#include "KismetProceduralMeshLibrary.h"
#include "../blender/proc_assets.h"
#include "../blender/rand.h"
#include "../blender/noise.h"
#include "Logging/StructuredLog.h"
#include "../proc_assets/Rock.h"

// Sets default values
AWorldGen::AWorldGen() 
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
	for (FFoliageParams & params: FoliageParams) {
		params.spawnRadius = math::min(params.spawnRadius, TerrainGrid.radius-1);
		params.initMesh(this, TerrainMesh);
	}
	
	
}
// Called when the game starts or when spawned
void AWorldGen::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = this->GetWorld();
	APlayerController* player = world->GetFirstPlayerController();
	PlayerPawn = player->GetPawn();
	resetCenter();
	reset();
	addChunksAroundPlayer();
	check(!isBusy);
}

bool AWorldGen::distributeFoliage(const int2 chunkAbsPos, const int sectionIdx, FFoliageType & foliage)
{
	FoliageChunk & chunk = foliage.cache.sections[sectionIdx];
	if (chunk.isPopulated) {
		return false;
	}
	const float2 size = float2(TerrainGrid.chunkSize, TerrainGrid.chunkSize);
	const float2 offset = float2(chunkAbsPos.X * TerrainGrid.chunkSize, chunkAbsPos.Y * TerrainGrid.chunkSize);
	const int count = math::min(int(foliage.density * TerrainGrid.chunkSize * TerrainGrid.chunkSize), 1000);
	const int seed = noise::hash(chunkAbsPos.X, chunkAbsPos.Y);
	blender::RandomNumberGenerator rng(seed);
	
	for (int i = 0; i < count; i++) {
		const float2 position = offset + size * rng.get_float2();
		float3 position3d = float3(position.X, position.Y, seaLevel);
		FRotator3d rot;
		if (foliage.alignToNormal) {
			const float3 gradient_and_height = get_height_with_derivative(position);
			const double3 normal = math::normalize(math::normal(double2(float2(gradient_and_height))));
			rot = normal.Rotation();
			position3d.Z += gradient_and_height.Z;
		}
		else {
			position3d.Z += get_height(position);
		}
		if (foliage.maxRotation > 0) {
			rot.Roll = rng.get_float() * foliage.maxRotation ;
		}
		foliageGenResult.Add(FTransform(rot, double3(position3d)));
		
		//params.addInstance(sectionIdx, FTransform(rot, double3(position3d)));
	}
	genResultFoliageIdx = foliage.idx;
	genResultFoliageTypeIdx = foliage.idx;
	chunk.isPopulated = true;
	return true;

}



// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);
	int2 playerPos = getPlayerAbsChunk();
	if (playerPos != absChunkOffset) {
		const int2 shift = absChunkOffset - playerPos;
		shiftSurroundingChunks(shift);
		absChunkOffset = playerPos;
		if(!isBusy)generateChunksAroundPlayerAsync();
		UE_LOGFMT(LogCore, Warning, "Status=\n{0}", toDebugStr());
		
	}
	
	

}