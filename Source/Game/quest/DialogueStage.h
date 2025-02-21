//MIT 
//Based on https://dev.epicgames.com/community/snippets/Qm2/unreal-engine-enhanced-input-double-tap-input-trigger
//Thanks to user colorindarkness 

#pragma once

#include "CoreMinimal.h"
#include "DialogueStage.generated.h"

class AGameCharacter;
class UDialogue;

UCLASS(BlueprintType)
class GAME_API UDialogueStage : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual void setup(AGameCharacter* npc, AGameCharacter* player, UDialogue* stage) const {};
	virtual const UDialogueStage* chooseOption(int i, AGameCharacter* npc, AGameCharacter* player, UDialogue* stage) const { return nullptr; };
	
};

