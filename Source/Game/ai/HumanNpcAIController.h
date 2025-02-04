// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HumanNpcAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AHumanNpcAIController : public AAIController
{
	GENERATED_BODY()
	
	void OnConstruction(const FTransform& Transform) override;

	virtual void OnPossess(APawn* pawn) override;

	void BeginPlay() override;
	
	void OnMoveComplete(FAIRequestID RequestID, const FPathFollowingResult& Result);

	void mainAILoop();
};
