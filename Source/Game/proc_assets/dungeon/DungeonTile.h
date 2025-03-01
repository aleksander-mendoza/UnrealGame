// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonTileType.h"
#include "DungeonTile.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FDungeonTile : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EDungeonTileType Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Style;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EComponentMobility::Type> Mobility = EComponentMobility::Static;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool HasCollisions = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool SimulatePhysics = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> Mesh;

	


};
