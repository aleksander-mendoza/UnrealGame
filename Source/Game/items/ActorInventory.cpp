// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorInventory.h"

// Sets default values for this component's properties
UActorInventory::UActorInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


void UActorInventory::PostInitProperties()
{
	Super::PostInitProperties();
	
}

// Called when the game starts
void UActorInventory::BeginPlay()
{
	Super::BeginPlay();

	
}


// Called every frame
void UActorInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UActorInventory::ResetToDefaultItems()
{
	for (int i = 0; i < DefaultItems.Instances.Num(); i++) {
		UItemObject* item = NewObject<UItemObject>();
		item->Instance = DefaultItems.Instances[i];
		Items.Add(item);

	}
}

void UActorInventory::ResetToDefaultClothes()
{
	for (int i = 0; i < DefaultClothes.Instances.Num(); i++) {
		UItemObject* item = NewObject<UItemObject>();
		item->Instance = DefaultClothes.Instances[i];
		Items.Add(item);
		const bool b = addClothingItem<true>(item);
		check(b);
	}
}

