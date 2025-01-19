// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResponseAction.h"
#include "DialogueResponse.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FDialogueResponse : public FTableRowBase
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ShortText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText LongText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EResponseAction> Responses;

};
