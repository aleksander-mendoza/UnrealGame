// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "RaceMenuEntryObject.h"
#include "RaceMenuControlEntry.generated.h"



/**
 * 
 */
UCLASS()
class GAME_API URaceMenuControlEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameLabel;
	

	UPROPERTY(meta = (BindWidget))
	USlider* ValueSlider;


	UPROPERTY(meta = (BindWidget))
	UButton* OpenColorPickerButton;

	URaceMenuEntryObject* EntryObject;

	UFUNCTION()
	void OnScalarValueChange(float v);


	UFUNCTION()
	void OpenColorPicker();

};
