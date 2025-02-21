// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"

AGameHUD::AGameHUD(){
}

void AGameHUD::BeginPlay()
{
	if(StatusWidget)StatusWidget->AddToViewport(9998);
	if (DialogueWidget) DialogueWidget->AddToViewport(9998);
}

void AGameHUD::hideInventory()
{
	if (InventoryInterface != nullptr) {
		if (InventoryInterface->Inventory != nullptr) {
			InventoryInterface->Inventory->InventoryWidget = nullptr;
		}
		InventoryInterface->RemoveFromParent();
		InventoryInterface = nullptr;
	}
}

