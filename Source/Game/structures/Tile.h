// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Tile.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FTile
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> Mesh;

	UPROPERTY()
	UInstancedStaticMeshComponent* InstancedMesh = nullptr;


	
};
