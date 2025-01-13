// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../structures/BuildingSystem.h"
#include "BuildInvEntryObject.generated.h"

UCLASS()
class GAME_API UBuildInvEntryObject : public UObject
{
	GENERATED_BODY()

public:

	int tileIdx;
	FBuildingSystem* bs;
};
