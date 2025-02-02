// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueOption.h"
#include "Dialogue.h"
#include "../../quest/DialogueDatabase.h"

void UDialogueOption::NativeConstruct()
{
	Super::NativeConstruct();
	OptionButton->OnClicked.AddUniqueDynamic(this, &UDialogueOption::OnOptionClick);
}

void UDialogueOption::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Option = Cast<UDialogueOptionObject>(ListItemObject);
	check(Option != nullptr);
	TextOption->SetText(Option->parent->stage->getOption(Option->i, Option->parent));
}

void UDialogueOption::OnOptionClick()
{
	if (Option) {
		const DialogueDatabase::DialogueStage * const nextStage = Option->parent->stage->chooseOption(Option->i, Option->parent);
		Option->parent->followUp(nextStage);
	}
}
