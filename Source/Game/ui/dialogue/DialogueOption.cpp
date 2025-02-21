// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueOption.h"
#include "Dialogue.h"
#include "../../quest/DialogueStage.h"

void UDialogueOption::NativeConstruct()
{
	Super::NativeConstruct();
	OptionButton->OnClicked.AddUniqueDynamic(this, &UDialogueOption::OnOptionClick);
}

void UDialogueOption::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!IsValid(Parent)) {
		UListViewBase* parent = GetOwningListView();
		Parent = Cast<UDialogue>(parent);
		check(IsValid(Parent));
	}
	Option = Cast<UDialogueOptionObject>(ListItemObject);
	check(Option != nullptr);
	TextOption->SetText(Option->text);
}

void UDialogueOption::OnOptionClick()
{
	if (Option) {
		Parent->chooseOption(Option->i);
	}
}
