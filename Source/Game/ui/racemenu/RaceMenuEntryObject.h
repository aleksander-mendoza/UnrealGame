// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RaceMenuEntryObject.generated.h"

class AGameCharacter;
class URaceMenu;
class URaceMenuControlEntry;
UCLASS()
class GAME_API URaceMenuEntryObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FText Name;
	
	AGameCharacter* Character;
	URaceMenu* menu;

	virtual float SetScalarValue(float val) { return val; };
	virtual void SetColorValue(FLinearColor rgb) {};
	virtual void Setup(URaceMenuControlEntry* entry) {};
	virtual void ResetToDefault() {};
	void Init(FText name, AGameCharacter* player, URaceMenu* racemenu) {
		Name = name;
		Character = player;
		menu = racemenu;
	}
};

UCLASS()
class GAME_API URaceMenuEntryObjectScalar : public URaceMenuEntryObject
{
	GENERATED_BODY()

public:

	float MinValue = 0, MaxValue = 1, Value = 0, DefaultValue = 0, StepSize = 0.001;

	virtual void Setup(URaceMenuControlEntry* entry) override;
	virtual float SetScalarValue(float val) override;
	virtual void ResetToDefault() override {
		SetScalarValue(DefaultValue);
	}
	inline void InitScalar(FText name, AGameCharacter* player, URaceMenu* racemenu, float value, float minValue = 0, float maxValue = 1, float defaultValue = 0, float stepSize=0.001) {
		Init(name, player, racemenu);
		MaxValue = maxValue;
		MinValue = minValue;
		DefaultValue = defaultValue;
		Value = value;
		StepSize = stepSize;
	}
};

UCLASS()
class GAME_API URaceMenuEntryObjectMorphTarget : public URaceMenuEntryObjectScalar
{
	GENERATED_BODY()

public:

	FName MorphTarget;
	

	virtual float SetScalarValue(float val) override;
	void InitMorphTarget(FText name, AGameCharacter* player, URaceMenu* racemenu, FName morphTarget, float minValue = 0, float maxValue = 1, float defaultValue = 0);
};


UCLASS()
class GAME_API URaceMenuEntryObjectHairdoPicker : public URaceMenuEntryObjectScalar
{
	GENERATED_BODY()

public:

	UDataTable * Hairdos;
	TArray<FName> HairdoNames;


	virtual float SetScalarValue(float val) override;
	void InitHairdoPicker(AGameCharacter* player, URaceMenu* racemenu);
};


UCLASS()
class GAME_API URaceMenuEntryObjectColorPicker : public URaceMenuEntryObject
{
	GENERATED_BODY()

public:

	FLinearColor DefaultValue;


	void InitColorPicker(FText name, AGameCharacter* player, URaceMenu* racemenu, FLinearColor defaultValue) {
		Init(name, player, racemenu);
		DefaultValue = defaultValue;
	}
	virtual void Setup(URaceMenuControlEntry* entry) override;
	virtual void ResetToDefault() override {
		SetColorValue(DefaultValue);
	}
	
};

UCLASS()
class GAME_API URaceMenuEntryObjectHairColorPicker : public URaceMenuEntryObjectColorPicker
{
	GENERATED_BODY()

public:

	void InitHairColorPicker(AGameCharacter* player, URaceMenu* racemenu);
	virtual void SetColorValue(FLinearColor rgb) override;
};