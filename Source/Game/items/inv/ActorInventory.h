// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../ItemInstance.h"
#include "ActorInventory.generated.h"

class ULoot;
//DECLARE_DELEGATE_OneParam(FStringDelegate, FString);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UActorInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UPROPERTY()
	class UInventory* InventoryWidget=nullptr;

	// Sets default values for this component's properties
	UActorInventory() {
		PrimaryComponentTick.bCanEverTick = false;
		bWantsInitializeComponent = true;
	}

	virtual void InitializeComponent() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<ULoot> Loot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray< TObjectPtr<UItemInstance>> Items;

	
	virtual void addItem(TObjectPtr<UItemInstance> item);
	virtual TObjectPtr<UItemInstance> removeItem(TObjectPtr<UItemInstance> item, int quantity = 1);
	virtual TObjectPtr<UItemInstance> dropItem(TObjectPtr<UItemInstance> item, int quantity = 1) {
		return removeItem(item, quantity);
	}

	virtual TObjectPtr<UItemInstance> spawnItem(UItem* itemType, int quantity = 1);
	
	

	virtual void clearInventory();

	virtual void resetInventory();
	void refreshInventoryEntryWidget(TObjectPtr<UItemInstance> item);
	void removeInventoryEntryWidget(TObjectPtr<UItemInstance> item);
	void addInventoryEntryWidget(TObjectPtr<UItemInstance> item);
};
