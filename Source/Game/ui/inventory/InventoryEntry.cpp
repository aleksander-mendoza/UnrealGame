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
	const char* c;
	switch (Item->equippedAt) {
		case EQUIPPED_AT_PROJECTILE:
			c = ">";
			break;
		case EQUIPPED_AT_DOUBLEHANDED:
			c = "LR";
			break;
		case EQUIPPED_AT_NONE:
			c = "";
			break;
		case EQUIPPED_AT_LEFT_HAND:
			c = "L";
			break;
		case EQUIPPED_AT_RIGHT_HAND:
			c = "R";
			break;
		default:
			c = "*";
			break;
	}
	StatusLabel->SetText(FText::FromString(c));
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
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UInventoryEntry::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey()==EKeys::R) {
		UInventory* inv = Item->container->currentWidget;
		Item->container->dropItem<true>(Item);
		inv->removeItem(Item);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}
