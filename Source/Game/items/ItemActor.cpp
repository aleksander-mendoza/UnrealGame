// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "../GameCharacter.h"
#include "../proc_assets/WorldGen.h"

// Sets default values
AItemActor::AItemActor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetSimulatePhysics(true);
	//Mesh->RegisterComponent();
	Mesh->SetupAttachment(RootComponent);
	this->SetActorHiddenInGame(true);
	PrimaryActorTick.bCanEverTick = false;

}

bool AItemActor::OnInteract(AGameCharacter* actor)
{
	actor->Inventory->addItem(Item);
	actor->worldGenRef->despawnItem(this);
	Item = nullptr;
	return true;
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

