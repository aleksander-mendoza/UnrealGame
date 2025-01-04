// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "../items/ActorInventory.h"
#include "Inventory.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UInventory : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	//virtual void NativeOnInitialized() override;
public:
	UActorInventory* Inventory;
	void setInventory(UActorInventory * Inventory);
	void removeItem(UItemObject* item) {
		ItemListView->RemoveItem(item);
	}
	void refresh() {
		ItemListView->RequestRefresh();
	}
protected:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UListView* ItemListView;
};
