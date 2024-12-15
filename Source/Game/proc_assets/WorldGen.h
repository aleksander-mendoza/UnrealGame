// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "WorldGen.generated.h"


UCLASS()
class GAME_API AWorldGen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGen();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float pointiness = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxHeight = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float scale = 500;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int numOfScales = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float scalingPowerBase = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int resolutionY = 32;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	int resolutionX=32;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	float  chunkW=100;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	float chunkH=100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int renderRadius = 8;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	float seaLevel=-10;

	UPROPERTY(BlueprintReadOnly) 
	UProceduralMeshComponent* TerrainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	UMaterialInterface* TerrainMaterial = nullptr;

private:
	int bottomChunkY;
	int leftmostChunkX;
	TArray<int> surroundingChunks;
	int getChunkIdx(int chunkX, int chunkY);
	int getSectionIdx(int chunkX, int chunkY);
	int setSectionIdx(int chunkX, int chunkY, int sectionIdx);
	void shiftSurroundingChunks(int shiftX, int shiftY);
	int getRenderDiameter();
	int getRenderArea();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Utils")
	void GenerateChunk(const int x, const int y);


};
