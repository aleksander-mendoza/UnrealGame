// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "../../GamePlayerController.h"

void UInventory::NativeConstruct()
{
    Super::NativeConstruct();
    SetIsFocusable(true);
    // Bind delegates here.
}

FReply UInventory::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::C) {
        GameController->CloseInventory();
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

void UInventory::setInventory(UCharacterInventory* inventory, AGamePlayerController* gameController)
{

    check(inventory != nullptr);
    GameController = gameController;
    Inventory = inventory;
    inventory->InventoryWidget = this;
    ItemListView->SetListItems(inventory->Items);
}
