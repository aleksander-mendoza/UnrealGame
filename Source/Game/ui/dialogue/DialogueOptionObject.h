// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../structures/BuildingSystem.h"
#include "DialogueOptionObject.generated.h"

UCLASS()
class GAME_API UDialogueOptionObject : public UObject
{
	GENERATED_BODY()

public:

	FText text;
	
};
