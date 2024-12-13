// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "TargetLockWidgetActor.generated.h"

UCLASS()
class GAME_API ATargetLockWidgetActor : public AActor
{
	GENERATED_BODY()
	
	
	

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* Widget;
	// Sets default values for this actor's properties
	ATargetLockWidgetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
