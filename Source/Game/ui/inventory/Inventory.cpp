// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

void UInventory::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind delegates here.
}

void UInventory::setInventory(UActorInventory * inventory)
{

    check(inventory != nullptr);
    inventory->currentWidget = this;
    this->Inventory = inventory;
    ItemListView->SetListItems(inventory->Items);
}
