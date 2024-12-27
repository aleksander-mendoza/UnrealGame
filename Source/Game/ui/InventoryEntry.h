// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "InventoryEntry.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UInventoryEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	// IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// IUserObjectListEntry

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ValueLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WarmthLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageLabel;
};
