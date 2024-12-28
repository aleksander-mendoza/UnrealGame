// Fill out your copyright notice in the Description page of Project Settings.


#include "FoliageParams.h"

void FFoliageParams::resetCache(int renderArea)
{
	cache.chunks.Empty();
	cache.chunks.Reserve(renderArea);
	while (cache.chunks.Num() < renderArea) {
		cache.chunks.AddDefaulted();
	}
	check(cache.chunks.Num() == renderArea);
}

void FFoliageParams::addInstance()
{
	check(InstancedMesh != nullptr);
	//this->InstancedMesh->Add
}
