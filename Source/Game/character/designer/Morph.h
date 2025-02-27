// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BodyPart.h"
#include "Morph.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FMorph : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName MorphName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EBodyPart BodyPart = EBodyPart::BODY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsForFemales = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsForMales = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Min=0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Max=1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Default=0;

};
