// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "DungeonGen.generated.h"


UCLASS()
class GAME_API ADungeonGen : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADungeonGen() {
		PrimaryActorTick.bCanEverTick = false;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector TileSize;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIntVector DungeonMapSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (RowType = "DungeonTile"))
	TSoftObjectPtr<UDataTable> TileTable;

	UPROPERTY()
	TArray<UInstancedStaticMeshComponent*> InstancedTiles;



	void PostInitializeComponents() override;
private:
	
	
public:
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



};