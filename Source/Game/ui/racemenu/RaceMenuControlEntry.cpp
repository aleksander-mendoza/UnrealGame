// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceMenuControlEntry.h"
#include "../../GameCharacter.h"
#include "RaceMenu.h"

void URaceMenuControlEntry::NativeConstruct()
{
	Super::NativeConstruct();
	ValueSlider->OnValueChanged.AddUniqueDynamic(this, &URaceMenuControlEntry::OnScalarValueChange);
	OpenColorPickerButton->OnClicked.AddUniqueDynamic(this, &URaceMenuControlEntry::OpenColorPicker);
}

void URaceMenuControlEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	URaceMenuEntryObject* Item = Cast<URaceMenuEntryObject>(ListItemObject);
	
	EntryObject = Item;
	NameLabel->SetText(Item->Name);
	Item->Setup(this);
	//ValueSlider->OnValueChanged.AddUniqueDynamic(Item, &URaceMenuEntryObject::SetValue); // idk why it doesnt work
}

void URaceMenuControlEntry::OnScalarValueChange(float v)
{
	if (EntryObject) {
		this->ValueSlider->SetValue(EntryObject->SetScalarValue(v));
	}
}

void URaceMenuControlEntry::OpenColorPicker()
{
	if (EntryObject) {
		EntryObject->menu->openColorPicker(EntryObject);
	}
}

