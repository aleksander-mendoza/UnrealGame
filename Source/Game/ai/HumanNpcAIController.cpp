// Fill out your copyright notice in the Description page of Project Settings.

#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "HumanNpcAIController.h"
void AHumanNpcAIController::OnConstruction(const FTransform& Transform) {
	UPathFollowingComponent* const e = GetPathFollowingComponent();
	e->OnRequestFinished.AddUObject(this, &AHumanNpcAIController::OnMoveComplete);
}
void AHumanNpcAIController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);
	
	
}

void AHumanNpcAIController::BeginPlay()
{
	Super::BeginPlay();
	mainAILoop();
	
}

void AHumanNpcAIController::OnMoveComplete(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	mainAILoop();
}

void AHumanNpcAIController::mainAILoop()
{
	const FVector origin = GetPawn()->GetActorLocation();
	if (UNavigationSystemV1* const nav = UNavigationSystemV1::GetCurrent(GetWorld())) {
		FNavLocation RandomLoc;
		if (nav->GetRandomPointInNavigableRadius(origin, 1500, RandomLoc)) {

			FAIMoveRequest movReq;
			movReq.SetGoalLocation(RandomLoc);
			FPathFollowingRequestResult movResult = MoveTo(movReq);
		}
	}
}
