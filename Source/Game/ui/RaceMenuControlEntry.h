// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "RaceMenuControlEntry.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API URaceMenuControlEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()


protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameLabel;
	

	UPROPERTY(meta = (BindWidget))
	USlider* ValueSlider;
};
