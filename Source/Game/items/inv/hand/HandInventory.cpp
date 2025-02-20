// Fill out your copyright notice in the Description page of Project Settings.



#include "HandInventory.h"
#include "Kismet/KismetSystemLibrary.h"


bool FHandInventory::HitDetectHand(AActor* owner, UWorld* world, USkeletalMeshComponent* const mesh, TArray<FHitResult>& OutHit)
{
	if (enableHandHitDetection) {
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
		objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(owner);
		FVector s, e;
		GetStartEndLocation(s, e, mesh);
		return UKismetSystemLibrary::SphereTraceMultiForObjects(world, s, e, 50., objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true);
	}
	return false;
}
