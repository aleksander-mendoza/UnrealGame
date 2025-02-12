// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActorSkeletal.h"
#include "../GameCharacter.h"
#include "../open_world/OpenWorld.h"

// Sets default values
AItemActorSkeletal::AItemActorSkeletal()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetSimulatePhysics(true);
	//Mesh->RegisterComponent();
	SetRootComponent(Mesh);

}
