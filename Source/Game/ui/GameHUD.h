// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Inventory.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UInventory> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> InventoryInterface;

	void showInventory();
	void hideInventory();
};
