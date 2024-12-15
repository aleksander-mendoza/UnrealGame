// Fill out your copyright notice in the Description page of Project Settings.


#include "Rock.h"
#include "KismetProceduralMeshLibrary.h"
#include <assert.h>
#include "../blender/proc_assets.h"

// Sets default values
ARock::ARock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RockMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RockMesh"));
	RockMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ARock::BeginPlay()
{
	Super::BeginPlay();
	proc_assets::Mesh mesh;
	proc_assets::cube_without_uvs(double3(0,0,0.5), double3(1,1,1), mesh);
	
	assert(mesh.vertices.Num()==mesh.uvs.Num());
	assert(mesh.vertices.Num() == mesh.normals.Num());
	assert(mesh.vertices.Num() == mesh.tangents.Num());
	this->RockMesh->CreateMeshSection_LinearColor(0, mesh.vertices, mesh.triangles, mesh.normals, mesh.uvs, TArray<FLinearColor>(), mesh.tangents, true);

	if (this->RockMaterial != nullptr) this->RockMesh->SetMaterial(0, this->RockMaterial);
}

// Called every frame
void ARock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

