// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RaceMenuEntryObject.generated.h"

UCLASS()
class GAME_API URaceMenuEntryObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FText Name;

	FName MorphTarget;

	float MinValue=0, MaxValue=1, Value=0, DefaultValue = 0, StepSize=0.001;

	USkeletalMeshComponent* Mesh;

	void SetValue(float val);
	inline void ResetToDefault() {
		SetValue(DefaultValue);
	}
};
