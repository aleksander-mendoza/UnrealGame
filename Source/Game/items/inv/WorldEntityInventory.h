// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryAndHealth.h"
#include "WorldEntityInventory.generated.h"

//DECLARE_DELEGATE_OneParam(FStringDelegate, FString);
class AItemActorProjectile;
class AGameCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UWorldEntityInventory  : public UInventoryAndHealth
{
	GENERATED_BODY()

	
public:	

	APawn* Pawn;

	virtual void BeginPlay() override;

	class AOpenWorld* worldRef;

	AItemActorProjectile*  shootProjectile(double speed);

	virtual TObjectPtr<UItemInstance>  dropItem(TObjectPtr<UItemInstance> item, int quantity) override;

	virtual void TickHealthBar() override;

	AGameCharacter* getPlayerCharacter() const;

	FVector getPlayerCameraLocation() const;
};
