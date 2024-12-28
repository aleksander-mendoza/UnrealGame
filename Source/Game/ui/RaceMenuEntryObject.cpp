// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceMenuEntryObject.h"

void URaceMenuEntryObject::SetValue(float val)
{
	UE_LOG(LogTemp, Warning, TEXT("Value changed"));
	Value = val;
	Mesh->SetMorphTarget(MorphTarget, val);
}
