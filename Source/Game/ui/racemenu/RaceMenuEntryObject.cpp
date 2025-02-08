// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceMenuEntryObject.h"
#include "../../GameCharacter.h"
#include "../../character/Hairdo.h"
#include "../../open_world/OpenWorld.h"
#include "RaceMenuControlEntry.h"
#include "RaceMenu.h"

float URaceMenuEntryObjectMorphTarget::SetScalarValue(float val)
{
	UE_LOG(LogTemp, Warning, TEXT("Value changed"));
	Value = val;
	Character->GetMesh()->SetMorphTarget(MorphTarget, val);
	return val;
}

void URaceMenuEntryObjectMorphTarget::InitMorphTarget(FText name, AGameCharacter* player, URaceMenu* racemenu, FName morphTarget, float minValue, float maxValue, float defaultValue)
{
	InitScalar(name, player, racemenu, player->GetMesh()->GetMorphTarget(morphTarget), minValue, maxValue, defaultValue, 0.001);
	this->MorphTarget = morphTarget;
}

void URaceMenuEntryObjectScalar::Setup(URaceMenuControlEntry* entry)
{
	entry->ValueSlider->SetMinValue(MinValue);
	entry->ValueSlider->SetMaxValue(MaxValue);
	entry->ValueSlider->SetValue(Value);
	entry->ValueSlider->SetStepSize(StepSize);
	entry->ValueSlider->SetVisibility(ESlateVisibility::Visible);
	entry->OpenColorPickerButton->SetVisibility(ESlateVisibility::Collapsed);
}

float URaceMenuEntryObjectScalar::SetScalarValue(float val)
{
	return this->Value = MinValue+floor((val-MinValue) / StepSize)* StepSize;
}


void URaceMenuEntryObjectHairColorPicker::InitHairColorPicker(AGameCharacter* player, URaceMenu* racemenu)
{
	InitColorPicker(FText::FromString("Hair color"), player, racemenu, player->getHairColor());
}


void URaceMenuEntryObjectColorPicker::Setup(URaceMenuControlEntry* entry)
{
	entry->ValueSlider->SetVisibility(ESlateVisibility::Collapsed);
	entry->OpenColorPickerButton->SetVisibility(ESlateVisibility::Visible);
}

void URaceMenuEntryObjectHairColorPicker::SetColorValue(FLinearColor rgb)
{
	Super::SetColorValue(rgb);
	Character->setHairColor(rgb);
}

float URaceMenuEntryObjectHairdoPicker::SetScalarValue(float val)
{
	int ival = int(val+0.5);
	Value = ival;
	FDataTableRowHandle handle;
	if (ival >= 0) {
		handle.DataTable = Hairdos;
		handle.RowName = HairdoNames[ival];
	}
	Character->setHairdo(handle);
	return Value;
}


void URaceMenuEntryObjectHairdoPicker::InitHairdoPicker(AGameCharacter* player, URaceMenu* racemenu)
{
	Hairdos = player->worldRef->getHairdos();
	HairdoNames = Hairdos->GetRowNames();
	FDataTableRowHandle hairdo = player->getHairdo();
	float value = -1;
	if (!hairdo.IsNull()) {
		value = HairdoNames.IndexOfByKey(hairdo.RowName);
	}
	InitScalar(FText::FromString("Hairdo style"), player, racemenu, value, -1, HairdoNames.Num()-1, value, 1.);
}
