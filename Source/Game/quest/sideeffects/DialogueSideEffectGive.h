//MIT 
//Based on https://dev.epicgames.com/community/snippets/Qm2/unreal-engine-enhanced-input-double-tap-input-trigger
//Thanks to user colorindarkness 

#pragma once

#include "CoreMinimal.h"
#include "../../items/loot/Loot.h"
#include "DialogueSideEffect.h"
#include "DialogueSideEffectGive.generated.h"

class UDialogue;
class AGameCharacter;

UCLASS(BlueprintType)
class GAME_API UDialogueSideEffectGive : public UDialogueSideEffect
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Quantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool GiveToNpc=false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<ULoot> Items;
	
	virtual bool apply(AGameCharacter* npc, AGameCharacter* player, UDialogue* stage) const override;


};

