// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameCharacter.h"
#include "GamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()
	

	/** MappingContext */
	UPROPERTY()
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY()
	UInputAction* JumpAction;
	/** Slow walk Input Action */
	UPROPERTY()
	UInputAction* SlowWalkAction;

	/** Run Input Action */
	UPROPERTY()
	UInputAction* RunAction;

	/** Move Input Action */
	UPROPERTY()
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY()
	UInputAction* LookAction;

	/** Lock Input Action */
	UPROPERTY()
	UInputAction* LockAction;

	/** Zoom In Input Action */
	UPROPERTY()
	UInputAction* ZoomAction;

	/** Attack Input Action */
	UPROPERTY()
	UInputAction* AttackAction;

	/** Attack Input Action */
	UPROPERTY()
	UInputAction* InteractAction;

	/** Open/Close Inventory Input Action */
	UPROPERTY()
	UInputAction* OpenInventoryAction;


	/** Open/Close Race Menu Input Action */
	UPROPERTY()
	UInputAction* OpenRaceMenuAction;



	virtual void SetupInputComponent() override;
	virtual void SetPawn(APawn * pawn) override;

	void Look(const FInputActionValue& Value);

	void TriggerInventory(const FInputActionValue& Value);
	void TriggerRaceMenu(const FInputActionValue& Value);
private:
	AGameCharacter* GameCharacter;
	UInputAction* MapKey(FKey key, EInputActionValueType type = EInputActionValueType::Boolean, bool triggerWhenPaused=false);
	void MapKey(UInputAction* act, FKey key, bool negateX, bool negateY, bool negateZ);
	void MapKey(UInputAction* act, FKey key, bool negate = false, bool swizzle = false, EInputAxisSwizzle order = EInputAxisSwizzle::YXZ);
};
