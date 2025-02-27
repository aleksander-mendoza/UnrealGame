// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceMenuEntryObject.h"
#include "../../GameCharacter.h"
#include "../../open_world/OpenWorld.h"
#include "RaceMenuControlEntry.h"
#include "RaceMenu.h"

float URaceMenuEntryObjectMorphTarget::SetScalarValue(float val)
{
	UE_LOG(LogTemp, Warning, TEXT("Value changed"));
	Value = val;
	Character->getPlayerMesh()->SetMorphTarget(MorphTarget, val);
	return val;
}

void URaceMenuEntryObjectMorphTarget::InitMorphTarget(FText name, UGameCharacterInventory* player, URaceMenu* racemenu, FName morphTarget, float minValue, float maxValue, float defaultValue)
{
	InitScalar(name, player, racemenu, player->getPlayerMesh()->GetMorphTarget(morphTarget), minValue, maxValue, defaultValue, 0.001);
	this->MorphTarget = morphTarget;
}

void URaceMenuEntryObjectScalar::Setup(URaceMenuControlEntry* entry)
{
	Super::Setup(entry);
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


void URaceMenuEntryObjectHairColorPicker::InitHairColorPicker(UGameCharacterInventory* player, URaceMenu* racemenu)
{
	InitColorPicker(FText::FromString("Hair color"), player, racemenu, player->getHairColor());
}


void URaceMenuEntryObjectColorPicker::Setup(URaceMenuControlEntry* entry)
{
	Super::Setup(entry);
	entry->ValueSlider->SetVisibility(ESlateVisibility::Collapsed);
	entry->OpenColorPickerButton->SetVisibility(ESlateVisibility::Visible);
}

void URaceMenuEntryObjectHairColorPicker::SetColorValue(FLinearColor rgb)
{
	Super::SetColorValue(rgb);
	Character->setHairColor(rgb);
}

void URaceMenuEntryObjectHairdoPicker::Setup(URaceMenuControlEntry* entry)
{
	MaxValue = Character->GetGender().Hairdos.Num()-1;
	Value = Character->getHairdo();
	Super::Setup(entry);
}

float URaceMenuEntryObjectHairdoPicker::SetScalarValue(float val)
{
	int ival = int(val+0.5);
	Value = ival;
	Character->setHairdo(ival);
	FFormatNamedArguments Args;
	Args.Add(TEXT("Hairdo"), Character->getHairdoName());
#define LOCTEXT_NAMESPACE "DesignerNamespace"
	FText text = FText::Format(LOCTEXT("HairLabel", "Hair: {Hairdo}"), Args);
#undef LOCTEXT_NAMESPACE
	Entry->NameLabel->SetText(text);
	return Value;
}


void URaceMenuEntryObjectHairdoPicker::InitHairdoPicker(UGameCharacterInventory* player, URaceMenu* racemenu)
{
	float value = player->getHairdo();
	InitScalar(FText::FromString("Hairdo:"), player, racemenu, value, -1, 0, value, 1.);
}
