// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"
#include <assert.h>
#include "KismetProceduralMeshLibrary.h"
#include "../blender/proc_assets.h"
#include "../blender/rand.h"
#include "../blender/noise.h"
#include "../proc_assets/Rock.h"

int AWorldGen::getChunkIdx(int chunkX, int chunkY) {
	int chunkYRelativeToRenderArea = chunkY - this->bottomChunkY;
	int chunkXRelativeToRenderArea = chunkX - this->leftmostChunkX;
	int diameter = getRenderDiameter();
	if (chunkYRelativeToRenderArea < 0 || chunkYRelativeToRenderArea >= diameter || chunkXRelativeToRenderArea < 0 || chunkXRelativeToRenderArea >= diameter)return -1;
	int chunkIdx = chunkXRelativeToRenderArea + diameter * chunkYRelativeToRenderArea;
	return chunkIdx;
}

int AWorldGen::getSectionIdx(int chunkX, int chunkY) {
	int chunkIdx = getChunkIdx(chunkX, chunkY);
	if (chunkIdx == -1)return -1;
	return this->surroundingChunks[chunkIdx];
}
int AWorldGen::setSectionIdx(int chunkX, int chunkY, int sectionIdx) {
	int chunkIdx = getChunkIdx(chunkX, chunkY);
	if (chunkIdx == -1)return -1;
	int prev = this->surroundingChunks[chunkIdx];
	this->surroundingChunks[chunkIdx] = sectionIdx;
	return prev;
}
void AWorldGen::shiftSurroundingChunks(int shiftX, int shiftY) {
	int diamater = getRenderDiameter();
	int offset = math::sgn(shiftX)+ math::sgn(shiftY)*diamater;
	for (int x = shiftX>0; x < diamater-(shiftX<0);x++) {
		for (int y = shiftY>0; y < diamater-(shiftY<0); y++) {
			int idx = x + y * diamater;
			surroundingChunks[idx] = surroundingChunks[idx + offset];
		}
	}
}
int AWorldGen::getRenderDiameter() {
	return 1 + this->renderRadius * 2;
}
int AWorldGen::getRenderArea() {
	int randerDiameter = this->getRenderDiameter();
	return randerDiameter * randerDiameter;
}
// Sets default values
AWorldGen::AWorldGen(): surroundingChunks()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	TerrainMesh->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void AWorldGen::BeginPlay()
{
	Super::BeginPlay();
	UWorld * world = this->GetWorld();
	APlayerController* player = world->GetFirstPlayerController();
	APawn* playerPawn = player->GetPawn();
	int chunkX=0, chunkY=0;
	if (playerPawn != nullptr) {
		FVector playerPosition = playerPawn->GetActorLocation();
		chunkX = floor(playerPosition.X / this->chunkW);
		chunkY = floor(playerPosition.Y / this->chunkH);
	}
	this->GenerateChunk(chunkX,chunkY);
	surroundingChunks.Empty();
	int renderArea = this->getRenderArea();
	surroundingChunks.Reserve(renderArea);
	while (surroundingChunks.Num() < renderArea)surroundingChunks.Add(-1);
}


void AWorldGen::GenerateChunk(const int x, const int y) {
	const float2 offsetf(x * this->chunkW, y * this->chunkH);
	const FVector offset = FVector(offsetf.X, offsetf.Y, this->seaLevel);
	const int resX = this->resolutionX;
	const int resY = this->resolutionY;
	const float2 size = float2(this->chunkW, this->chunkH);
	proc_assets::Mesh mesh;
	//proc_assets::morenoise(offset, resX, resY, size, mesh, scale, pointiness, scalingPowerBase, numOfScales, maxHeight);
	proc_assets::perlin(offset, resX, resY, size, mesh, scale, maxHeight);
	blender::RandomNumberGenerator rng(noise::hash(x,y));
	for (int i = 0; i < 30; i++) {
		const float2 position = offsetf + size * rng.get_float2();
		//const float3 gradient_and_height = noise::morenoise(position, scale, pointiness, scalingPowerBase, numOfScales)* maxHeight;
		const float3 gradient_and_height = noise::perlin_noise_derivative(position, scale) * maxHeight;
		FActorSpawnParameters SpawnInfo;
		const double3 normal = double3(math::normalize(math::normal(float2(gradient_and_height))));
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

		const double3 position3dZ = position3d + double3(rockDensity)* normal;
		ARock* rock3 = GetWorld()->SpawnActor<ARock>(ARock::StaticClass(), position3dZ, rot, SpawnInfo);
		rock3->SetActorLabel(FString::Printf(TEXT("ARock %d Z"), i));
	
	}

	int idx = 0;
	this->TerrainMesh->CreateMeshSection_LinearColor(idx, mesh.vertices, mesh.triangles, mesh.normals, mesh.uvs, TArray<FLinearColor>(), mesh.tangents, true);

	if (this->TerrainMaterial != nullptr) this->TerrainMesh->SetMaterial(idx, this->TerrainMaterial);

}

//void AWorldGen::GenerateChunk(const int x, const int y) {
//	const float invScale = 1. / scale;
//	const float chunkWScaled = this->chunkW * invScale;
//	const float chunkHScaled = this->chunkH * invScale;
//	const float startX = x * this->chunkW;
//	const float startY = y * this->chunkH;
//	const float startXScaled = startX * invScale;
//	const float startYScaled = startY * invScale;
//	const float endXScaled = startXScaled + chunkWScaled;
//	const float endYScaled = startYScaled + chunkHScaled;
//	const int resX = this->resolutionX;
//	const int resY = this->resolutionY;
//	const float spacingX = this->chunkW / (float)(resX-1);
//	const float spacingY = this->chunkH / (float)(resY-1);;
//	const int64 startXScaledInt = int64(startXScaled);
//	const int64 startYScaledInt = int64(startYScaled);
//	const int64 endXScaledInt = int64(endXScaled) + 1;
//	const int64 endYScaledInt = int64(endYScaled) + 1;
//	const int32 widthScaledInt = endXScaledInt - startXScaledInt;
//	const int32 heightScaledInt = endYScaledInt - startYScaledInt;
//	const int32 scaledAreaInt = widthScaledInt*heightScaledInt;
//	float * randomMatrix = new float[scaledAreaInt];
//	
//	int32 i = 0;
//	for (int64 vy = startYScaledInt; vy < endYScaledInt; vy++) {
//		for (int64 vx = startXScaledInt; vx < endYScaledInt; i++, vx++) {
//			randomMatrix[i] = noise::hash_to_float(vx, vy) * maxHeight;
//		}
//	}
//	
//	TArray<FVector> vertices;
//	TArray<FVector2D> uvs;
//	TArray<int32> triangles;
//	TArray<FVector> normals;
//	TArray<FProcMeshTangent> tangents;
//	for (int32 vy = 1; vy < resY; vy++) {
//		const float relativeY = vy * spacingY;
//		const float vertex_Y = startY + relativeY;
//		const float relativeYScaled = relativeY * invScale;
//		const int32 relativeYScaledInt = int32(relativeYScaled);
//		const float fractionYScaled = relativeYScaled - float(relativeYScaledInt);
//		
//		for (int32 vx = 0; vx < resX; vx++) {
//			const float relativeX = vx * spacingX;
//			const float vertex_X = startX + relativeX;
//			const float relativeXScaled = relativeX * invScale;
//			const int32 relativeXScaledInt = int32(relativeXScaled);
//			const float fractionXScaled = relativeXScaled - float(relativeXScaledInt);
//			const int32 idx_bot = relativeXScaledInt + relativeYScaledInt * widthScaledInt;
//			const float perlin_bot_left = randomMatrix[idx_bot];
//			const float perlin_bot_right = randomMatrix[idx_bot+1];
//			const int32 idx_top = idx_bot + widthScaledInt;
//			const float perlin_top_left = randomMatrix[idx_top];
//			const float perlin_top_right = randomMatrix[idx_top+1];
//			float perlin_value = noise::mix(perlin_bot_left, perlin_bot_right, perlin_top_left, perlin_top_right, fractionXScaled, fractionYScaled);
//			float2 perlin_gradient = noise::mix_derivative(perlin_bot_left, perlin_bot_right, perlin_top_left, perlin_top_right, fractionXScaled, fractionYScaled);
//
//			float vertex_Z = seaLevel + perlin_value;
//			vertices.Add(double3(vertex_X, vertex_Y, vertex_Z));
//			FVector2D uv(vertex_X, vertex_Y);
//			uvs.Add(uv);
//			float3 normal = math::normalize(math::normal(perlin_gradient));
//			normals.Add(FVector(normal));
//			float3 tangent = math::normalize(math::tangent(perlin_gradient));
//			tangents.Add(FProcMeshTangent(FVector(tangent), false));
//			
//		}
//	}
//	
//	
//	for (int32 vy = 0; vy < resY - 1; vy++) {
//		for (int32 vx = 0; vx < resX-1; vx++) {
//			const int bottomLeft = vx + vy * resX;
//			const int bottomRight = bottomLeft + 1;
//			const int topLeft = bottomLeft + resX;
//			const int topRight = topLeft + 1;
//			triangles.Add(bottomLeft);
//			triangles.Add(topLeft);
//			triangles.Add(bottomRight);
//
//			triangles.Add(bottomRight);
//			triangles.Add(topLeft);
//			triangles.Add(topRight);
//		}
//	}
//	
//	int idx = 0;
//	this->TerrainMesh->CreateMeshSection_LinearColor(idx, vertices, triangles, normals, uvs, TArray<FLinearColor>(), tangents, true);
//	if(this->TerrainMaterial!=nullptr) this->TerrainMesh->SetMaterial(idx, this->TerrainMaterial);
//	
//}

// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

