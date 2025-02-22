// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "InventoryEntry.h"
#include "../../GamePlayerController.h"

void UInventory::NativeConstruct()
{
    Super::NativeConstruct();
    SetIsFocusable(true);
    // Bind delegates here.
}

FReply UInventory::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    const FKey key = InKeyEvent.GetKey();
    if (key == EKeys::C) {
        GameController->CloseInventory();
        return FReply::Handled();
    } else if(key == EKeys::R) {
        UItemInstance* item = Cast<UItemInstance>(ItemListView->GetSelectedItem());
        if (item) {
            item->drop();
            return FReply::Handled();
        }
    }
    else if (key == EKeys::E) {
        UItemInstance* item = Cast<UItemInstance>(ItemListView->GetSelectedItem());
        if (item) {
            item->use(true);
            return FReply::Handled();
        }
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

void UInventory::updateItem(UItemInstance* item) {
    UInventoryEntry * row = ItemListView->GetEntryWidgetFromItem<UInventoryEntry>(item);
    if (row) {
        check(row->Item == item);
        row->refresh();
    }
}