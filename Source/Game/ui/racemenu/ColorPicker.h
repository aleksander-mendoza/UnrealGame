// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Synth2DSlider.h"
#include "Components/Slider.h"
#include "ColorPicker.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorValueChangedEvent, FLinearColor, rgb);
/**
 * 
 */
UCLASS()
class GAME_API UColorPicker : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	USynth2DSlider* Color2DSlider;


	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	USlider* BrightnessSlider;
	

	UFUNCTION()
	void OnColorValueChange(float x);

	UFUNCTION()
	void OnBrightnessValueChange(float x);

	FLinearColor rgb;
	FLinearColor hsv;

public:
	/** Called when the value is changed by slider or typing. */
	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FOnColorValueChangedEvent OnColorValueChanged;

};
