// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "NpcHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UNpcHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Health;

	
};
