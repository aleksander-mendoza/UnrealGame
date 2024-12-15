// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Rock.generated.h"

UCLASS()
class GAME_API ARock : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UProceduralMeshComponent* RockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* RockMaterial = nullptr;

	// Sets default values for this actor's properties
	ARock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
