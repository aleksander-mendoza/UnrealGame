// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "DialogueOptionObject.h"
#include "DialogueOption.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UDialogueOption : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;



	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextOption;


	UPROPERTY(meta = (BindWidget))
	UButton* OptionButton;

	UPROPERTY()
	class UDialogueOptionObject* Option;

	UFUNCTION()
	void OnOptionClick();
};
