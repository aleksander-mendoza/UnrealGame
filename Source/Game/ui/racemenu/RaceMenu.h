// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "../../GameCharacter.h"
#include "ColorPicker.h"
#include "RaceMenu.generated.h"

class URaceMenuEntryObject;
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


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true", RowType = "Hairdo"))
	UDataTable* Hairdos;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UListView* ControlListView;


	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UColorPicker* ColorPicker;

	URaceMenuEntryObject* ColorPickerSubscriber;

	UFUNCTION()
	void OnColorChanged(FLinearColor rgb);


public:
	void setSliderValues(AGameCharacter* character);
	inline void openColorPicker(URaceMenuEntryObject* entry) {
		if (ColorPickerSubscriber == entry) {
			ColorPicker->SetVisibility(ESlateVisibility::Collapsed);
			ColorPickerSubscriber = nullptr;
		}
		else {
			ColorPicker->SetVisibility(ESlateVisibility::Visible);
			ColorPickerSubscriber = entry;
		}
	}
private:
	void AddMorphTargetEntry(AGameCharacter* player, FName morphTarget, FString name, float minValue = 0, float maxValue = 1, float defaultValue = 0);

	
};
