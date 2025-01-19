// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorld.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Logging/StructuredLog.h"

// Sets default values
AOpenWorld::AOpenWorld() 
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	

	
}

void AOpenWorld::PostInitializeComponents()
{ 
	Super::PostInitializeComponents();
	
	
}
// Called when the game starts or when spawned
void AOpenWorld::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = this->GetWorld();
	APlayerController* player = world->GetFirstPlayerController();
	AGameModeBase* gameMode = UGameplayStatics::GetGameMode(GetWorld());
	AActor * start = gameMode->FindPlayerStart(player, TEXT("TestStart"));
		//
	double3 startLoc = start->GetActorLocation();
	FRotator startRot = start->GetActorRotation();
	PlayerPawn = world->SpawnActor<AGameCharacter>(PlayerPawnClass, startLoc, startRot);
	player->Possess(PlayerPawn);
	PlayerPawn->worldRef = this;

}



// Called every frame
void AOpenWorld::Tick(float DeltaTime)
{
	

}