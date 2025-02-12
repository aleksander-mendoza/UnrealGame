// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "../GameCharacter.h"
#include "../open_world/OpenWorld.h"

// Sets default values
AItemActor::AItemActor()
{
	this->SetActorHiddenInGame(true);
	PrimaryActorTick.bCanEverTick = false;

}

bool AItemActor::OnInteract(AGameCharacter* actor)
{
	actor->Inventory->addItem(Item);
	actor->worldRef->despawnItem(this);
	Item = nullptr;
	return true;
}
