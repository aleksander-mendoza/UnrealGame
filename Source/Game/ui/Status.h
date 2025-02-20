// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

#include "Status.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UStatus : public UUserWidget
{
	GENERATED_BODY()

	//virtual void NativeConstruct() override;
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Health;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Stamina;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Arousal;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Magic;

	float HealthFullTime = 0;
	float StaminaFullTime = 0;
	float MagicFullTime = 0;

	//struct FHealth * healthComponenet;
};
