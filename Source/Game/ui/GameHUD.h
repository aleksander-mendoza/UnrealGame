// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "inventory/Inventory.h"
#include "building/BuildingInventory.h"
#include "racemenu/RaceMenu.h"
#include "Status.h"
#include "../GameCharacter.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AGameHUD : public AHUD
{
	GENERATED_BODY()

protected:
	AGameHUD();
	virtual void BeginPlay();
public:

	

	UPROPERTY(EditAnywhere, Category = "User Interface")
	TObjectPtr<UStatus> StatusWidget;

	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<URaceMenu> RaceMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<URaceMenu> RaceMenuInterface;

	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UInventory> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UInventory> InventoryInterface;


	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UBuildingInventory> BuildingInventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UBuildingInventory> BuildingInventoryInterface;

	inline TObjectPtr<UBuildingInventory> showBuildingInventory(AGameCharacter* GameCharacter) {
		BuildingInventoryInterface = CreateWidget<UBuildingInventory>(GetWorld(), BuildingInventoryWidgetClass);
		//BuildingInventoryInterface->setInventory(GameCharacter->Inventory);
		BuildingInventoryInterface->AddToViewport(9999); // Z-order, this just makes it render on the very top.
		return BuildingInventoryInterface;
	}
	inline TObjectPtr<UInventory> showInventory(AGameCharacter* GameCharacter) {
		InventoryInterface = CreateWidget<UInventory>(GetWorld(), InventoryWidgetClass);
		InventoryInterface->setInventory(GameCharacter->Inventory);
		InventoryInterface->AddToViewport(9999); // Z-order, this just makes it render on the very top.
		return InventoryInterface;
	}
	inline TObjectPtr<URaceMenu> showRaceMenu(AGameCharacter* GameCharacter) {
		RaceMenuInterface = CreateWidget<URaceMenu>(GetWorld(), RaceMenuWidgetClass);
		RaceMenuInterface->setSliderValues(GameCharacter);
		RaceMenuInterface->AddToViewport(9999); // Z-order, this just makes it render on the very top.
		return RaceMenuInterface;
	}
	inline void hideInventory() {
		InventoryInterface->Inventory->currentWidget = nullptr;
		InventoryInterface->RemoveFromParent();
		InventoryInterface = nullptr;
	}
	inline void hideBuildingInventory() {
		BuildingInventoryInterface->RemoveFromParent();
		BuildingInventoryInterface = nullptr;
	}
	inline void hideRaceMenu() {
		RaceMenuInterface->RemoveFromParent();
		RaceMenuInterface = nullptr;
	}
	inline bool isInventoryOpen() const {
		return InventoryInterface != nullptr;
	}
	inline bool canOpenInventory() const {
		return IsValid(InventoryWidgetClass);
	}
	inline bool isBuildingInventoryOpen() const {
		return BuildingInventoryInterface != nullptr;
	}
	inline bool canOpenBuildingInventory() const {
		return IsValid(BuildingInventoryWidgetClass);
	}
	inline bool isRaceMenuOpen() const {
		return RaceMenuInterface != nullptr;
	}
	inline bool canOpenRaceMenu() const {
		return IsValid(RaceMenuWidgetClass);
	}
};
