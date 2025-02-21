//MIT 
//Based on https://dev.epicgames.com/community/snippets/Qm2/unreal-engine-enhanced-input-double-tap-input-trigger
//Thanks to user colorindarkness 

#pragma once

#include "CoreMinimal.h"
#include "DialogueStage.h"
#include "DialogueResponse.h"
#include "DialogueLine.generated.h"


UCLASS(BlueprintType)
class GAME_API UDialogueLine : public UDialogueStage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Text;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDialogueResponse> Responses;
	virtual void setup(AGameCharacter* npc, AGameCharacter* player, UDialogue* stage) const override;
	virtual const UDialogueStage* chooseOption(int i, AGameCharacter* npc, AGameCharacter* player, UDialogue* stage) const override;
};
