// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldGen.h"
#include "KismetProceduralMeshLibrary.h"
#include "blender/noise.h"

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
	noise::test();
}


void AWorldGen::GenerateChunk(const int x, const int y) {
	const FVector offset = FVector(x*this->chunkW,y*this->chunkH,this->seaLevel);
	const int resX = this->resolutionX;
	const int resY = this->resolutionY;
	const float spacingX = this->chunkW / (float)(resX-1);
	const float spacingY = this->chunkH / (float)(resY-1);
	TArray<FVector> vertices;
	TArray<FVector2D> uvs;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;

	for (int32 vy = 0; vy < resY; vy++) {
		for (int32 vx = 0; vx < resX; vx++) {
			FVector vertex = offset;
			vertex.X += vx * spacingX;
			vertex.Y += vy * spacingY;
			//vertex.Z += noise::perlin_noise(float2(vertex.X / scale, vertex.Y / scale)) * maxHeight;
			float perlin_value;
			float2 gradient = math::mul(noise::perlin_noise_derivative(float2(vertex.X / scale, vertex.Y / scale), &perlin_value), maxHeight);
			vertex.Z += perlin_value * maxHeight;
			vertices.Add(vertex);
			FVector2D uv(vertex);
			uvs.Add(uv);
			float3 normal = math::normalize(math::normal(gradient));
			normals.Add(FVector(normal));
			float3 tangent = math::normalize(math::tangent(gradient));
			tangents.Add(FProcMeshTangent(FVector(tangent), false));
			
		}
	}
	
	
	for (int32 vy = 0; vy < resY - 1; vy++) {
		for (int32 vx = 0; vx < resX-1; vx++) {
			const int bottomLeft = vx + vy * resX;
			const int bottomRight = bottomLeft + 1;
			const int topLeft = bottomLeft + resX;
			const int topRight = topLeft + 1;
			triangles.Add(bottomLeft);
			triangles.Add(topLeft);
			triangles.Add(bottomRight);

			triangles.Add(bottomRight);
			triangles.Add(topLeft);
			triangles.Add(topRight);
		}
	}
	
	int idx = 0;
	this->TerrainMesh->CreateMeshSection_LinearColor(idx, vertices, triangles, normals, uvs, TArray<FLinearColor>(), tangents, true);
	if(this->TerrainMaterial!=nullptr) this->TerrainMesh->SetMaterial(idx, this->TerrainMaterial);
	
}

// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

