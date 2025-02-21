
#include "DialogueSideEffectGive.h"
#include "../../GameCharacter.h"
#include "../../ui/dialogue/Dialogue.h"

bool UDialogueSideEffectGive::apply(AGameCharacter* npc, AGameCharacter* player, UDialogue* stage) const
{
	ULoot * loot = Items.LoadSynchronous();
	AGameCharacter* target = GiveToNpc ? npc : player;
	loot->sample(target->GameMovement->Inventory, Quantity);
	return true;
}
