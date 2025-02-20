// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryEntry.h"
#include "../../items/inv/CharacterInventory.h"
#include "Inventory.h"
#include "../../items/item/Item.h"


void UInventoryEntry::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UInventoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UListViewBase* parent = GetOwningListView();
	OwningInventory = Cast<UInventory>(parent);
	check(IsValid(OwningInventory));
	Item = Cast<UItemInstance>(ListItemObject);
	
	if (Item->Quantity > 1) {
		FString n = Item->ItemType->Name.ToString();
		n.AppendChar('(');
		n += FString::FromInt(Item->Quantity);
		n.AppendChar(')');
		NameLabel->SetText(FText::FromString(n));
	}
	else {
		NameLabel->SetText(Item->ItemType->Name);
	}
	setStatus();
	WeightLabel->SetText(FText::AsNumber(Item->ItemType->Weight));
	ValueLabel->SetText(FText::AsNumber(Item->ItemType->Value));
	WarmthLabel->SetText(FText::AsNumber(Item->ItemType->getWarmth()));
	DamageLabel->SetText(FText::AsNumber(Item->ItemType->getDamage()));
	
}
void UInventoryEntry::setStatus() {
	const char* c;
	switch (Item->EquippedAt) {
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
		if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {
			if (Item->use(OwningInventory->Inventory, true)) {
				OwningInventory->refresh();
			}
		}
		else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {
			if (Item->use(OwningInventory->Inventory, false)) {
				OwningInventory->refresh();
			}
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UInventoryEntry::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey()==EKeys::R) {
		if (Item->drop(OwningInventory->Inventory)) {
			OwningInventory->removeItem(Item);	
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}
