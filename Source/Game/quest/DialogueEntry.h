// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueResponse.h"
#include "DialogueEntry.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FDialogueEntry : public FTableRowBase
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDialogueResponse> Responses;

};
