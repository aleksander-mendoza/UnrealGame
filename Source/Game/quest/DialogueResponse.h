//MIT 
//Based on https://dev.epicgames.com/community/snippets/Qm2/unreal-engine-enhanced-input-double-tap-input-trigger
//Thanks to user colorindarkness 

#pragma once

#include "CoreMinimal.h"
#include "DialogueResponse.generated.h"

class UDialogueStage;
class UDialogueSideEffect;

USTRUCT(BlueprintType)
struct FDialogueResponse {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Text;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UDialogueStage> Next;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UDialogueSideEffect>> SideEffects;
};
