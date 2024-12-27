// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.h"
#include "ItemInstanceList.generated.h"
/**
 *
 */
USTRUCT()
struct GAME_API FItemInstanceList : public FTableRowBase
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FItemInstance> Instances;

};
