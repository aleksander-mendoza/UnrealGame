#include "DialogueDatabase.h"
#include "../ui/dialogue/Dialogue.h"



namespace DialogueDatabase {
	void GiveItem::sideEffect(UDialogue* dialogue) const
	{
		//dialogue->player
	}

	const TextBased2Stage INITIALIZE_ENSLAVEMENT_CONVERSATION("",
		"You are a slave now!", nullptr,
		"I'll spare you this time.", nullptr);
	const TextBased0Stage RUMOURS("They say that Hilda's son is evil.");
	const TextBased3Stage INITIALIZE_GENERIC_CONVERSATION("Hello!", 
		"What have you got for sale?", nullptr, 
		"Follow me.", nullptr, 
		"Heard any rumours lately?", &RUMOURS);
	
}
