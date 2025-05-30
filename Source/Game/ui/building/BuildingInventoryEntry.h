// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildInvEntryObject.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "BuildingInventoryEntry.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UBuildingInventoryEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameLabel;



	UBuildInvEntryObject* EntryObject;

};
