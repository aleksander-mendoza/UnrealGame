// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "RaceMenu.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API URaceMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UListView* ControlListView;


};
