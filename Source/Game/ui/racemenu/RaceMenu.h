// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "../../GameCharacter.h"
#include "ColorPicker.h"
#include "RaceMenu.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API URaceMenu : public UUserWidget
{
	GENERATED_BODY()

	

	//virtual void NativeOnInitialized() override;
protected:

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UListView* ControlListView;


	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UColorPicker* ColorPicker;
public:
	void setSliderValues(AGameCharacter* character);
private:
	void AddEntry(USkeletalMeshComponent* mesh, FName morphTarget, FString name, float minValue = 0, float maxValue = 1, float defaultValue = 0);

	
};
