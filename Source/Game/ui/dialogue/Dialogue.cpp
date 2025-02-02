// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue.h"
#include "../../GameCharacter.h"
#include "../../GamePlayerController.h"
#include "../../quest/DialogueDatabase.h"

void UDialogue::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	SetVisibility(ESlateVisibility::Collapsed);
}

FReply UDialogue::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{

	return FReply::Unhandled();
}

void UDialogue::setup(AGameCharacter* Npc, AGameCharacter* Player, const DialogueDatabase::DialogueStage* Stage)
{
	SetVisibility(ESlateVisibility::Visible);
	this->npc = Npc;
	this->player = Player;
	SpeakerName->SetText(npc->GetCharacterName());
	followUp(Stage);
}

void UDialogue::followUp(const DialogueDatabase::DialogueStage* const nextStage)
{
	stage = nextStage;
	while (true) {
		if (stage == nullptr) {
			this->player->GameController->CloseDialogue();
			SetVisibility(ESlateVisibility::Collapsed);
			lineLifetime = 0;
			return;
		}
		else {
			const int optionCount = stage->optionCount(this);
			switch (optionCount ) {
			case -1:
				stage = stage->chooseOption(0, this);
				break;
			case 0:
				showText(stage->getText(this));
				lineLifetime = DIALOGUE_LINE_DURATION;
				this->player->GameController->CloseDialogue();
				return;
			default:
				clearOptions();
				showText(stage->getText(this));
				for (int j = EntryPool.Num(); j < optionCount; j++) {
					UDialogueOptionObject* item = NewObject<UDialogueOptionObject>(this);
					item->i = j;
					item->parent = this;
					EntryPool.Add(item);
				}
				for (int j = 0; j < optionCount; j++) {
					ResponseOptions->AddItem(EntryPool[j]);
				}
				return;
			}
		}
	}
}
