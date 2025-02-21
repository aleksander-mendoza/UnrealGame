// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "DialogueOptionObject.h"
#include "Dialogue.generated.h"

class UDialogueStage;
class AGameCharacter;

#define DIALOGUE_LINE_DURATION 3
/**
 * 
 */
UCLASS()
class GAME_API UDialogue : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	//virtual void NativeOnInitialized() override;
public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void setup(AGameCharacter* Npc, AGameCharacter* Player, const UDialogueStage* Stage);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeakerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeakerText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UListView* ResponseOptions;

	UPROPERTY()
	TArray<UDialogueOptionObject *> EntryPool;

	UPROPERTY()
	AGameCharacter* Npc; 
	
	UPROPERTY()
	AGameCharacter* Player;

	UPROPERTY()
	const UDialogueStage* Stage;

	inline void showText(FText text) {
		SpeakerText->SetText(text);
	}
	inline void showLine(FText name, FText text, float duration = DIALOGUE_LINE_DURATION) {
		SetVisibility(ESlateVisibility::Visible);
		lineLifetime = duration;
		SpeakerName->SetText(name);
		showText(text);
	}
	inline void clearOptions() {
		ResponseOptions->ClearListItems();
	}
	
	void followUp(const UDialogueStage* const nextStage);
	void ensureCapacity(int responseItemCount);
	void addResponseItem(FText text);
	void closeDialogue();
	void chooseOption(int i);
	float lineLifetime = 0;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
