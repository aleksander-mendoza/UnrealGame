// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldEntityInventory.h"
#include "../../open_world/OpenWorld.h"
#include "Kismet/KismetMathLibrary.h"
#include "../actor/ItemActorProjectile.h"
#include "Kismet/GameplayStatics.h"

void UWorldEntityInventory::BeginPlay()
{  
	Super::BeginPlay();
	if (worldRef == nullptr) {
		worldRef = Cast<AOpenWorld>(UGameplayStatics::GetActorOfClass(GetWorld(), AOpenWorld::StaticClass()));
		check(worldRef != nullptr);
	}
	Pawn = Cast<APawn>(GetOwner());
	check(Pawn != nullptr);


}


AItemActorProjectile* UWorldEntityInventory::shootProjectile(double speed) {
	UItemInstance* projectileItem = decrementProjectileCount();
	if (projectileItem) {
		return worldRef->shootProjectile(this, projectileItem, speed);
	}
	return nullptr;
}

TObjectPtr<UItemInstance> UWorldEntityInventory::dropItem(TObjectPtr<UItemInstance> item, int quantity) {
	TObjectPtr<UItemInstance> popped = Super::dropItem(item, quantity);
	worldRef->spawnItem(popped, GetOwner()->GetActorLocation(), FRotator());
	return popped;
}

void UWorldEntityInventory::TickHealthBar()
{
	Super::TickHealthBar();
	FVector camPos = getPlayerCameraLocation();
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), camPos);
	HealthBarComponent->SetWorldRotation(rot);
	
}


AGameCharacter* UWorldEntityInventory::getPlayerCharacter() const
{
	check(worldRef != nullptr);
	return worldRef->PlayerPawn;
}

FVector UWorldEntityInventory::getPlayerCameraLocation() const
{
	return getPlayerCharacter()->GetCurrentCamera()->GetComponentLocation();
}
