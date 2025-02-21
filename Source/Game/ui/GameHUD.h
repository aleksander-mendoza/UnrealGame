// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "inventory/Inventory.h"
#include "building/BuildingInventory.h"
#include "dialogue/Dialogue.h"
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
	TObjectPtr<UDialogue> DialogueWidget;

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
	inline TObjectPtr<UInventory> showInventory(AGameCharacter* GameCharacter, AGamePlayerController * ctrl) {
		InventoryInterface = CreateWidget<UInventory>(GetWorld(), InventoryWidgetClass);
		InventoryInterface->setInventory(GameCharacter->GameMovement->Inventory, ctrl);
		InventoryInterface->AddToViewport(9999); // Z-order, this just makes it render on the very top.
		InventoryInterface->SetKeyboardFocus();
		return InventoryInterface;
	}
	inline void showLine(FText name, FText text) {
		DialogueWidget->showLine(name, text);
	}
	inline TObjectPtr<UDialogue> showDialogue(AGameCharacter* npc, AGameCharacter* player, const UDialogueStage* stage) {
		DialogueWidget->setup(npc, player, stage);
		return DialogueWidget;
	}
	inline TObjectPtr<URaceMenu> showRaceMenu(AGameCharacter* GameCharacter) {
		RaceMenuInterface = CreateWidget<URaceMenu>(GetWorld(), RaceMenuWidgetClass);
		RaceMenuInterface->setSliderValues(GameCharacter);
		RaceMenuInterface->AddToViewport(9999); // Z-order, this just makes it render on the very top.
		RaceMenuInterface->SetKeyboardFocus();
		return RaceMenuInterface;
	}
	inline void hideDialogue() {
		DialogueWidget->clearOptions();
	}
	void hideInventory();
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
	inline bool canOpenDialogue() const {
		
		return IsValid(DialogueWidget);
	}
	
	
};
