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
	TerrainMesh->SetMobility(EComponentMobility::Static);
	TerrainMesh->bUseAsyncCooking = true;
}

void AWorldGen::PostInitializeComponents()
{ 
	Super::PostInitializeComponents();
	
	for (int i = 0; i < EntityParams.Num();i++) {
		FEntityParams& params = EntityParams[i];
		
		params.initMesh(this, TerrainMesh, i);
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
	update();
	if (!isBusy) {
		if (applyResults() && (couldHaveMoreChunksToLoad || playerCrossedChunkBoundary)) {
			isBusy = true;
			AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
				{
					couldHaveMoreChunksToLoad = generateChunksAroundPlayer<true>();
					isBusy = false;
				}
			);
			
		}
	}
	

}