// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetLockWidgetActor.h"

// Sets default values
ATargetLockWidgetActor::ATargetLockWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Target Widget"));
	Widget->SetDrawSize(FVector2D(30, 30));
	Widget->SetWidgetSpace(EWidgetSpace::Screen);

}

// Called when the game starts or when spawned
void ATargetLockWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetLockWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

