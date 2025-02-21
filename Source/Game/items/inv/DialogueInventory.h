// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorInventory.h"

#include "DialogueInventory.generated.h"

class UDialogueStage;
//DECLARE_DELEGATE_OneParam(FStringDelegate, FString);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UDialogueInventory: public UActorInventory
{
	GENERATED_BODY()

public:	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDialogueStage> DialogueStage;

	

};
