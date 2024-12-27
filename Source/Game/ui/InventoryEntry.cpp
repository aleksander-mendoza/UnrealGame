// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryEntry.h"
#include "../items/ItemObject.h"


void UInventoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	const UItemObject* Item = Cast<UItemObject>(ListItemObject);
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
	WeightLabel->SetText(FText::AsNumber(row->Weight));
	ValueLabel->SetText(FText::AsNumber(row->Value));
	WarmthLabel->SetText(FText::AsNumber(row->Warmth));
	DamageLabel->SetText(FText::AsNumber(row->Damage));
}
