// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../ItemInstance.h"
#include "ActorInventory.generated.h"

//DECLARE_DELEGATE_OneParam(FStringDelegate, FString);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UActorInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	// Sets default values for this component's properties
	UActorInventory() {
		PrimaryComponentTick.bCanEverTick = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray< TObjectPtr<UItemInstance>> Items;

	
	virtual void addItem(TObjectPtr<UItemInstance> item) {
		check(item->EquippedAt == EQUIPPED_AT_NONE);
		Items.Add(item);
	} 
	virtual TObjectPtr<UItemInstance> removeItem(TObjectPtr<UItemInstance> item, int quantity = 1);
	virtual TObjectPtr<UItemInstance> dropItem(TObjectPtr<UItemInstance> item, int quantity = 1) {
		return removeItem(item, quantity);
	}

	virtual TObjectPtr<UItemInstance> spawnItem(UItem* itemType, int quantity = 1);
	

};
