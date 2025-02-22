#include "DialogueLine.h"
#include "../ui/dialogue/Dialogue.h"
#include "sideeffects/DialogueSideEffect.h"
#include "../GameCharacter.h"



void UDialogueLine::setup(AGameCharacter* npc, AGameCharacter* player, UDialogue* stage) const
{
	stage->clearOptions();
	stage->ensureCapacity(Responses.Num());
	for (int i = 0; i < Responses.Num(); i++) {
		stage->addResponseItem(Responses[i].Text);
	}
	stage->showText(Text);
	stage->lineLifetime = 0;
}


const UDialogueStage* UDialogueLine::chooseOption(int i, AGameCharacter* npc, AGameCharacter* player, UDialogue* stage) const
{
	const FDialogueResponse & resp = Responses[i];
	for (int j = 0; j < resp.SideEffects.Num(); j++) {
		if (!resp.SideEffects[i]->apply(npc, player, stage)) {
			break;
		}
	}
	return resp.Next.LoadSynchronous();
}
