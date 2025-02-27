// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceMenu.h"
#include "RaceMenuEntryObject.h"
#include "../../items/inv/GameCharacterInventory.h"

void URaceMenu::NativeConstruct()
{
	ColorPicker->OnColorValueChanged.AddUniqueDynamic(this, &URaceMenu::OnColorChanged);
	ColorPicker->SetVisibility(ESlateVisibility::Collapsed);
	AssCategoryButton->OnClicked.AddUniqueDynamic(this, &URaceMenu::OnAssCategory);
	BreastCategoryButton->OnClicked.AddUniqueDynamic(this, &URaceMenu::OnBreastCategory);
	PussyCategoryButton->OnClicked.AddUniqueDynamic(this, &URaceMenu::OnPussyCategory);
	FemaleLegsCategoryButton->OnClicked.AddUniqueDynamic(this, &URaceMenu::OnFemaleLegsCategory);
	FemaleArmsCategoryButton->OnClicked.AddUniqueDynamic(this, &URaceMenu::OnFemaleArmsCategory);
	FemaleBodyCategoryButton->OnClicked.AddUniqueDynamic(this, &URaceMenu::OnFemaleBodyCategory);
	FemaleFaceCategoryButton->OnClicked.AddUniqueDynamic(this, &URaceMenu::OnFemaleFaceCategory);
	MaleGenderButton->OnClicked.AddUniqueDynamic(this, &URaceMenu::OnMaleGender);
	FemaleGenderButton->OnClicked.AddUniqueDynamic(this, &URaceMenu::OnFemaleGender);

}
void URaceMenu::OnColorChanged(FLinearColor rgb)
{
	if (ColorPickerSubscriber)ColorPickerSubscriber->SetColorValue(rgb);
}

void URaceMenu::setupAllEntries(UGameCharacterInventory* character) {
	
	Player = character;
	AllEntries.Empty();


	URaceMenuEntryObjectHairdoPicker* hairdoStyleEntry = NewObject<URaceMenuEntryObjectHairdoPicker>();
	hairdoStyleEntry->InitHairdoPicker(character, this);
	hairdoStyleEntry->bodyPart = EBodyPart::HEAD;
	AllEntries.Add(hairdoStyleEntry);

	URaceMenuEntryObjectHairColorPicker* hairColorEntry = NewObject<URaceMenuEntryObjectHairColorPicker>();
	hairColorEntry->InitHairColorPicker(character, this);
	hairColorEntry->bodyPart = EBodyPart::HEAD;
	AllEntries.Add(hairColorEntry);
	if (character->Race != nullptr && !character->Race->Morphs.IsNull()) {
		UDataTable* table = character->Race->Morphs.LoadSynchronous();
		for (auto row : table->GetRowMap()) {
			FMorph* morph = (FMorph*)row.Value;
			URaceMenuEntryObjectMorphTarget* entry = NewObject<URaceMenuEntryObjectMorphTarget>();
			entry->InitMorphTarget(morph->Name, character, this, morph->MorphName, morph->Min, morph->Max, morph->Default);
			entry->bodyPart = morph->BodyPart;
			entry->IsForFemales = morph->IsForFemales;
			entry->IsForMales = morph->IsForMales;
			AllEntries.Add(entry);

		}
	}

	refreshEntries();
	
}

void URaceMenu::refilterEntries(EBodyPart bodyPart)
{
	refilterEntries(bodyPart, Player->IsFemale());
}
void URaceMenu::refreshEntries()
{
	ControlListView->ClearListItems();
	for (int i = 0; i < AllEntries.Num(); i++) {
		URaceMenuEntryObject* entry = AllEntries[i];
		if (entry->bodyPart == currentBodypart && (Player->IsFemale() ? entry->IsForFemales : entry->IsForMales)) {
			ControlListView->AddItem(entry);
		}
	}
}
void URaceMenu::refilterEntries(EBodyPart bodyPart, bool isFemale)
{
	bool isDirty = false;
	if (isFemale != Player->IsFemale()) {
		Player->SetGender(isFemale);
		isDirty = true;
	}
	if (currentBodypart != bodyPart) {
		currentBodypart = bodyPart;
		isDirty = true;
	}
	if (isDirty) {
		refreshEntries();
	}
	
}

