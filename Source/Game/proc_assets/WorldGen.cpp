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
	Super::PostInitializeComponents();
	FoliageParams.Sort([](const FFoliageParams& ip1, const FFoliageParams& ip2) {
		return  ip1.spawnRadius > ip2.spawnRadius;
	});
	for (FFoliageParams & params: FoliageParams) {
		params.spawnRadius = math::min(params.spawnRadius, renderRadius-1);
		params.InstancedMesh = NewObject<UInstancedStaticMeshComponent>(this,UInstancedStaticMeshComponent::StaticClass(), TEXT("FoliageMesh"));
		params.InstancedMesh->bDisableCollision = !params.hasCollisions;
		params.InstancedMesh->SetRemoveSwap();
		params.InstancedMesh->SetStaticMesh(params.Mesh);
		params.InstancedMesh->RegisterComponent();
		params.InstancedMesh->AttachToComponent(TerrainMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	
}
void AWorldGen::resetSurroundingChunks() {
	this->TerrainMesh->ClearAllMeshSections();
	surroundingChunks.Empty();
	unusedSectionIndices.Empty();
	int renderArea = this->getRenderArea();
	surroundingChunks.Reserve(renderArea);
	unusedSectionIndices.Reserve(renderArea);
	for (FFoliageParams& params : FoliageParams) {
		params.resetCache(renderArea);
	}
	while (surroundingChunks.Num() < renderArea) {
		unusedSectionIndices.Add(surroundingChunks.Num());
		surroundingChunks.Add(-1);
		check(unusedSectionIndices.Last() >= 0);
	}
	check(surroundingChunks.Num() == renderArea);
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
	generateChunksAroundPlayer(false);
	check(requestQueue.IsEmpty());
	check(!isMeshGenResultReady);
	check(!isBusy);
}

void AWorldGen::distributeFoliage(const MeshGenRequest& r, const int foliageIdx)
{
	FFoliageParams& params = FoliageParams[foliageIdx];
	FoliageChunk & chunk = params.cache.sections[r.sectionIdx];
	if (chunk.isPopulated) {
		return;
	}
	const float2 size = float2(this->chunkW, this->chunkH);
	const float2 offset = float2(r.chunkAbsPos.X * this->chunkW, r.chunkAbsPos.Y * this->chunkH);
	const int count = math::min(int(params.density * chunkW * chunkH), 1000);
	const int seed = noise::hash(r.chunkAbsPos.X, r.chunkAbsPos.Y);
	blender::RandomNumberGenerator rng(seed);
	
	for (int i = 0; i < count; i++) {
		const float2 position = offset + size * rng.get_float2();
		float3 position3d = float3(position.X, position.Y, seaLevel);
		FRotator3d rot;
		if (params.alignToNormal) {
			const float3 gradient_and_height = get_height_with_derivative(position);
			const double3 normal = math::normalize(math::normal(double2(float2(gradient_and_height))));
			rot = normal.Rotation();
			position3d.Z += gradient_and_height.Z;
		}
		else {
			position3d.Z += get_height(position);
		}
		if (params.maxRotation > 0) {
			rot.Roll = rng.get_float() * params.maxRotation ;
		}
		params.addInstance(r.sectionIdx, FTransform(rot, double3(position3d)));
	}
	chunk.isPopulated = true;

}

void AWorldGen::generateChunksInRadius(int2 chunkAbsPos, int radius, int resX, int resY, bool async) {
	generateChunk(chunkAbsPos, resX, resY, async, 0);
	for (int dist = 1; dist <= radius; dist++) {
		for (int x = chunkAbsPos.X - dist; x <= chunkAbsPos.X + dist; x++) {
			generateChunk(int2(x, chunkAbsPos.Y + dist), resX, resY, async, dist);
			generateChunk(int2(x, chunkAbsPos.Y - dist), resX, resY, async, dist);
		}
		for (int y = chunkAbsPos.Y - dist + 1; y < chunkAbsPos.Y + dist; y++) {
			generateChunk(int2(chunkAbsPos.X - dist, y), resX, resY, async, dist);
			generateChunk(int2(chunkAbsPos.X + dist, y), resX, resY, async, dist);
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
				MeshGenRequest r = i;
				meshGenResult.mesh.clearEverythingButTriangles();
				generateChunk(r, meshGenResult);
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
		absChunkOffset = playerPos;
		generateChunksAroundPlayer(true);
		UE_LOGFMT(LogCore, Warning, "Status=\n{0}", toDebugStr());
		
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