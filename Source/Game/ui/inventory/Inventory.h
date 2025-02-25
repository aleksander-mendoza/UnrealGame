// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../items/ItemInstance.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Inventory.generated.h"

class AGamePlayerController;
class UCharacterInventory;
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
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	UCharacterInventory* Inventory;
	AGamePlayerController* GameController;
	void setInventory(UCharacterInventory* Inventory, AGamePlayerController* GameController);
	void removeItem(UItemInstance* item) {
		ItemListView->RemoveItem(item);
	}
	void addItem(UItemInstance* item) {
		ItemListView->AddItem(item);
	}
	void updateItem(UItemInstance* item);
	void refresh() {
		ItemListView->RegenerateAllEntries();
	}
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UListView* ItemListView;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UHorizontalBox* Header;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* NpcButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* PlayerButton;


	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* NpcName;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* PlayerName;
	
};
