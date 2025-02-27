// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/Button.h"
#include "ColorPicker.h"
#include "../../character/designer/BodyPart.h"
#include "RaceMenu.generated.h"

class UGameCharacterInventory;
class URaceMenuEntryObject;
class URaceMenuControlEntry;
/**
 * 
 */
UCLASS()
class GAME_API URaceMenu : public UUserWidget
{
	GENERATED_BODY()

	

	//virtual void NativeOnInitialized() override;
protected:
	virtual void NativeConstruct() override;

	EBodyPart currentBodypart = EBodyPart::BODY;
	UGameCharacterInventory* Player;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UListView* ControlListView;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton * AssCategoryButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* BreastCategoryButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* PussyCategoryButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* FemaleLegsCategoryButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* FemaleArmsCategoryButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* FemaleBodyCategoryButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* FemaleFaceCategoryButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* MaleGenderButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* FemaleGenderButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UColorPicker* ColorPicker;

	URaceMenuEntryObject* ColorPickerSubscriber;
	UPROPERTY()
	TArray<URaceMenuEntryObject*> AllEntries;

	UFUNCTION()
	void OnColorChanged(FLinearColor rgb);
	UFUNCTION()
	void OnAssCategory(){ refilterEntries(EBodyPart::ASS ); }
	UFUNCTION()
	void OnBreastCategory() { refilterEntries(EBodyPart::BREASTS  ); }
	UFUNCTION()
	void OnPussyCategory() { refilterEntries(EBodyPart::GENITALS  ); }
	UFUNCTION()
	void OnFemaleLegsCategory() { refilterEntries(EBodyPart::LEGS  ); }
	UFUNCTION()
	void OnFemaleArmsCategory() { refilterEntries(EBodyPart::ARMS  ); }
	UFUNCTION()
	void OnFemaleBodyCategory() { refilterEntries(EBodyPart::BODY  ); }
	UFUNCTION()
	void OnFemaleFaceCategory() { refilterEntries(EBodyPart::HEAD  ); }
	UFUNCTION()
	void OnMaleGender() { refilterEntries(currentBodypart, false); }
	UFUNCTION()
	void OnFemaleGender() { refilterEntries(currentBodypart, true); }


public:
	void setupAllEntries(UGameCharacterInventory* character);
	void refilterEntries(EBodyPart bodyPart, bool isFemale);
	void refilterEntries(EBodyPart bodyPart);
	void refreshEntries();
	inline void openColorPicker(URaceMenuEntryObject* entry) {
		ColorPickerSubscriber = ColorPickerSubscriber == entry ? nullptr : entry;
		ColorPicker->SetVisibility(ColorPickerSubscriber == nullptr ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
private:
	

	
};
