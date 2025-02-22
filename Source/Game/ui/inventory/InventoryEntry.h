// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "InventoryEntry.generated.h"


/**
 * 
 */
UCLASS()
class GAME_API UInventoryEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

	
protected:
	virtual void NativeConstruct() override;
	// IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// IUserObjectListEntry
	
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
public:
	class UItemInstance* Item;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameLabel;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatusLabel;

	UPROPERTY(meta = (BindWidget))
	UBorder* RowBorder;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ValueLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WarmthLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageLabel;

	void setStatus();
	void refresh();
};
