// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "../blender/utildefines.h"
#include "BuildingSystem.generated.h"


UCLASS()
class GAME_API ABuildingSystem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABuildingSystem();
	
	float tileSize; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTile> Tiles;


	inline void putTile(const int tileIdx, const int2 position) {
		const float2 pos(position.X* tileSize, position.Y* tileSize);
		FTile& t = Tiles[tileIdx];
		if (t.InstancedMesh == nullptr) {

		}
	}
	void PostInitializeComponents() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};