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
	Item = Cast<UItemInstance>(ListItemObject);
	refresh();
	
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
void UInventoryEntry::refresh()
{
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
void UInventoryEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	RowBorder->SetBrushColor(FLinearColor(1, 1, 1, bIsSelected ? 0.4 : 0));
}

FReply UInventoryEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	
	if (Item != nullptr) {
		if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {
			Item->use(true);
		}
		else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {
			Item->use(false);
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UInventoryEntry::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (IsValid(Item)) {
		UListView* lv = Cast<UListView>(GetOwningListView());
		lv->SetSelectedItem(Item);
	}
}

void UInventoryEntry::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{

}

FReply UInventoryEntry::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	
	return FReply::Unhandled();
}
