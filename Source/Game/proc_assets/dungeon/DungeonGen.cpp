// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGen.h"
#include "DungeonTile.h"
#include "KismetProceduralMeshLibrary.h"


void ADungeonGen::PostInitializeComponents()
{ 
	Super::PostInitializeComponents();
	if (TileTable != nullptr) {
		for (auto row : TileTable->GetRowMap()) {
			FDungeonTile* tile = (FDungeonTile*)row.Value;

			UInstancedStaticMeshComponent* mesh = NewObject<UInstancedStaticMeshComponent>(this, UInstancedStaticMeshComponent::StaticClass());
			mesh->bDisableCollision = !tile->HasCollisions;
			mesh->SetRemoveSwap();
			mesh->SetSimulatePhysics(tile->SimulatePhysics);
			mesh->SetStaticMesh(tile->Mesh);
			mesh->SetMobility(tile->Mobility);
			mesh->RegisterComponent();
			mesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

			InstancedTiles.Add(mesh);
		}
	}

	int mapSize = DungeonMapSize.X * DungeonMapSize.Y * DungeonMapSize.Z;
	TArray<int> map;
	map.Reserve(mapSize);
	for (int i = 0; i < mapSize; i++) {
		map.Add(-2);
	}
}
// Called when the game starts or when spawned
void ADungeonGen::BeginPlay()
{
	Super::BeginPlay();

	
}

