//MIT 
//Based on https://dev.epicgames.com/community/snippets/Qm2/unreal-engine-enhanced-input-double-tap-input-trigger
//Thanks to user colorindarkness 

#pragma once

#include "CoreMinimal.h"

class UDialogue;

namespace DialogueDatabase {
	class DialogueStage {
	public:
		virtual FText getText(UDialogue* dialogue) const = 0;
		/**If option count is -1 then chooseOption(0, dialogue) 
		is executed immediately without waiting for user to choose any option.
		This is useful for chaining several dialogue stages one after another that are meant
		to cause some side-effects (giving user some item, making NPCs aggressive, spawning NPCs, 
		teleporting player etc). If the chooseOption returns a nullptr then the conversation ends.*/
		virtual int optionCount(UDialogue * dialogue) const = 0 ;
		virtual FText getOption(int i, UDialogue* dialogue) const = 0;
		/**returning null will end the conversation*/
		virtual const DialogueStage * chooseOption(int i, UDialogue* dialogue) const = 0;
	
	};
	class NonTextBasedStage :public  DialogueStage {
	public:
		const  DialogueStage* nextStage;
		NonTextBasedStage(const  DialogueStage* followUp) : nextStage(followUp) {}

		FText getText(UDialogue* dialogue) const override { return FText(); };
		int optionCount(UDialogue* dialogue) const override { return -1; }
		FText getOption(int i, UDialogue* dialogue)const override { return FText(); };
		const DialogueStage* chooseOption(int i, UDialogue* dialogue)const override { sideEffect(dialogue);  return nextStage; };
		virtual void sideEffect(UDialogue* dialogue) const = 0;
	};

	class GiveItem :public  NonTextBasedStage {
	public:
		GiveItem(const  DialogueStage* followUp) : NonTextBasedStage(followUp){
		}
		void sideEffect(UDialogue* dialogue) const override;
			
	};


	class TextBased0Stage :public  DialogueStage {
	public:
		const char* const mainText;
		TextBased0Stage(const char * const text): mainText(text){}
		FText getText(UDialogue* dialogue) const override { return FText::FromString(mainText); }
		int optionCount(UDialogue* dialogue) const override { return 0; }
		FText getOption(int i, UDialogue* dialogue) const override { return FText(); }
		const DialogueStage* chooseOption(int i, UDialogue* dialogue) const override { return nullptr; }
	};

	class TextBased1Stage :public  DialogueStage {
	public:
		const char* const  mainText;
		const char* option0;
		const  DialogueStage* stage0;
		TextBased1Stage(const char* const  text, const char * response, const  DialogueStage * followUp) : mainText(text), option0(response) {}
		FText getText(UDialogue* dialogue) const override { return FText::FromString(mainText); }
		int optionCount(UDialogue* dialogue)const override { return 1; }
		FText getOption(int i, UDialogue* dialogue)const override { return FText::FromString(option0); }
		const DialogueStage* chooseOption(int i, UDialogue* dialogue)const override { return stage0; }
	};

	class TextBased2Stage :public  DialogueStage {
	public:
		const char* const mainText;
		const char* option[2];
		const DialogueStage* stage[2];
		TextBased2Stage(const char* const text, const char* first, const  DialogueStage* firstFollowUp, const char* second, const  DialogueStage* secondFollowUp) :
			mainText(text) {
			option[0] = first;
			option[1] = second;
			stage[0] = firstFollowUp;
			stage[1] = secondFollowUp;
		}
		FText getText(UDialogue* dialogue) const override { return FText::FromString(mainText); }
		int optionCount(UDialogue* dialogue) const override { return 2; }
		FText getOption(int i, UDialogue* dialogue) const override { return FText::FromString(option[i]); }
		const DialogueStage* chooseOption(int i, UDialogue* dialogue) const override { return stage[i]; }
	};

	class TextBased3Stage : public DialogueStage {
	public:
		const char* const mainText;
		const char* option[3];
		const DialogueStage* stage[3];
		TextBased3Stage(const char* const text, const char* first, const DialogueStage* firstFollowUp, const char* second, const DialogueStage* secondFollowUp, const char* third, const DialogueStage* thirdFollowUp) :
			mainText(text) {
			option[0] = first;
			option[1] = second;
			option[2] = third;
			stage[0] = firstFollowUp;
			stage[1] = secondFollowUp;
			stage[2] = thirdFollowUp;
		}
		FText getText(UDialogue* dialogue) const override { return FText::FromString(mainText); }
		int optionCount(UDialogue* dialogue) const override { return 3; }
		FText getOption(int i, UDialogue* dialogue) const override { return FText::FromString(option[i]); }
		const DialogueStage* chooseOption(int i, UDialogue* dialogue)const override { return stage[i]; }
	};

	extern const TextBased2Stage INITIALIZE_ENSLAVEMENT_CONVERSATION;
	extern const TextBased3Stage INITIALIZE_GENERIC_CONVERSATION;

}
