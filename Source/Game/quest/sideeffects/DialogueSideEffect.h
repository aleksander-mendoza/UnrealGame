//MIT 
//Based on https://dev.epicgames.com/community/snippets/Qm2/unreal-engine-enhanced-input-double-tap-input-trigger
//Thanks to user colorindarkness 

#pragma once

#include "CoreMinimal.h"
#include "DialogueSideEffect.generated.h"

class UDialogue;
class AGameCharacter;

UCLASS(BlueprintType)
class GAME_API UDialogueSideEffect : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	virtual bool apply(AGameCharacter* npc, AGameCharacter* player, UDialogue* stage) const { return true; };


};

