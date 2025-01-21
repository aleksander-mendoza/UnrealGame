// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryEntry.h"
#include "../../items/ActorInventory.h"
#include "Inventory.h"
#include "../../items/ItemObject.h"


void UInventoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Item = Cast<UItemObject>(ListItemObject);
	const FItem* row = Item->Instance.getRow();
	if (Item->Instance.Quantity > 1) {
		FString n = row->Name.ToString();
		n.AppendChar('(');
		n += FString::FromInt(Item->Instance.Quantity);
		n.AppendChar(')');
		NameLabel->SetText(FText::FromString(n));
	}
	else {
		NameLabel->SetText(row->Name);
	}
	setStatus();
	WeightLabel->SetText(FText::AsNumber(row->Weight));
	ValueLabel->SetText(FText::AsNumber(row->Value));
	WarmthLabel->SetText(FText::AsNumber(row->Warmth));
	DamageLabel->SetText(FText::AsNumber(row->Damage));
}
void UInventoryEntry::setStatus() {
	if (Item->equippedAt == -4) {
		StatusLabel->SetText(FText::FromString("LR"));
	}
	else if (Item->equippedAt == -3) {
		StatusLabel->SetText(FText::FromString(""));
	}
	else if (Item->equippedAt == -2) {
		StatusLabel->SetText(FText::FromString("L"));
	}
	else if (Item->equippedAt == -1) {
		StatusLabel->SetText(FText::FromString("R"));
	}
	else if (Item->equippedAt >= 0) {
		StatusLabel->SetText(FText::FromString("*"));
	}
}
void UInventoryEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	//OnMouseButtonDown
}

FReply UInventoryEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (Item != nullptr) {
		check(Item->container != nullptr);
		if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {
			Item->container->toggleItem(Item, true, true, false);
			Item->container->currentWidget->refresh();
		}
		else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {
			Item->container->toggleItem(Item, false, true, false);
			Item->container->currentWidget->refresh();
		}
	}
	return FReply::Handled();
}

FReply UInventoryEntry::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey()==EKeys::R) {
		UInventory* inv = Item->container->currentWidget;
		Item->container->dropItem<true>(Item);
		inv->removeItem(Item);
	}
	return FReply::Handled();
}
