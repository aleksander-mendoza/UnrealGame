// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue.h"
#include "../../GameCharacter.h"
#include "../../GamePlayerController.h"
#include "../../quest/DialogueStage.h"

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

void UDialogue::setup(AGameCharacter* npc, AGameCharacter* player, const UDialogueStage* stage)
{
	SetVisibility(ESlateVisibility::Visible);
	this->Npc = npc;
	this->Player = player;
	SpeakerName->SetText(npc->GetCharacterName());
	followUp(stage);
}

void UDialogue::followUp(const UDialogueStage* const nextStage)
{
	Stage = nextStage;
	if (Stage == nullptr) {
		closeDialogue();
	}
	else {
		nextStage->setup(Npc, Player, this);
	}
}

void UDialogue::ensureCapacity(int responseItemCount)
{
	for (int j = EntryPool.Num(); j < responseItemCount; j++) {
		UDialogueOptionObject* item = NewObject<UDialogueOptionObject>(this);
		item->i = j;
		EntryPool.Add(item);
	}
}

void UDialogue::addResponseItem(FText text)
{
	int j = ResponseOptions->GetNumItems();
	UDialogueOptionObject* item = EntryPool[j];
	item->text = text;
	check(item->i == j);
	ResponseOptions->AddItem(item);

}

void UDialogue::closeDialogue()
{
	Player->GameController->CloseDialogue();
}

void UDialogue::chooseOption(int i)
{
	if (Stage) {
		followUp(Stage->chooseOption(i, Npc, Player, this));
		
	}
}

void UDialogue::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (lineLifetime > 0) {
		lineLifetime -= InDeltaTime;
		if (lineLifetime <= 0) {
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
