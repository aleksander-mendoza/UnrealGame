// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DialogueOptionObject.generated.h"
class UDialogue;

UCLASS()
class GAME_API UDialogueOptionObject : public UObject
{
	GENERATED_BODY()

public:
	int i;
	FText text;
	class UDialogue* Parent;
};
