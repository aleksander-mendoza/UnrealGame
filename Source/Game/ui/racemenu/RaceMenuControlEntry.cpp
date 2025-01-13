// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceMenuControlEntry.h"

void URaceMenuControlEntry::NativeConstruct()
{
	Super::NativeConstruct();
	ValueSlider->OnValueChanged.AddUniqueDynamic(this, &URaceMenuControlEntry::OnValueChange);
	
}

void URaceMenuControlEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	URaceMenuEntryObject* Item = Cast<URaceMenuEntryObject>(ListItemObject);
	
	EntryObject = Item;
	NameLabel->SetText(Item->Name);
	ValueSlider->SetMinValue(Item->MinValue);
	ValueSlider->SetMaxValue(Item->MaxValue);
	ValueSlider->SetValue(Item->Value);
	//ValueSlider->OnValueChanged.AddUniqueDynamic(Item, &URaceMenuEntryObject::SetValue); // idk why it doesnt work
}

void URaceMenuControlEntry::OnValueChange(float v)
{
	if (EntryObject)EntryObject->SetValue(v);
}
