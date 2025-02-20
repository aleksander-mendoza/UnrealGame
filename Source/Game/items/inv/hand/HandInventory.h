// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnyMesh.h"
#include "AnySocket.h"
#include "../../item/Item.h"
#include "HandInventory.generated.h"



USTRUCT(BlueprintType)
struct GAME_API FHandInventory
{
	GENERATED_BODY()


	UPROPERTY()
	FAnyMesh HandMesh;

	UPROPERTY()
	FAnySocket BladeStart;
	
	UPROPERTY()
	FAnySocket BladeEnd;
	
	UPROPERTY()
	const USkeletalMeshSocket* BareHandSocket;

	bool enableHandHitDetection;
	bool enableFootHitDetection;
	
	inline bool IsSheathed() const {
		return isSheathed;
	}
	bool isSheathed = true;
	
	inline FAnySocket & getBladeSocket(bool start) {
		return start ? BladeStart : BladeEnd;
	}
	inline const FAnySocket & getBladeSocket(bool start) const{
		return start ? BladeStart : BladeEnd;
	}

	inline FVector getHandLocation(const USkeletalMeshComponent* const playerMesh) const {
		return BareHandSocket->GetSocketLocation(playerMesh);
	}
	inline FTransform getBladeTransform(bool start) const {
		return getBladeSocket(start).GetSocketTransform(HandMesh);
	}
	inline FVector getBladeLocation(bool start) const {
		return getBladeTransform(start).GetLocation();
	}
	inline void GetStartEndLocation(FVector& start, FVector& end, const USkeletalMeshComponent* const playerMesh) const {
		if (BladeStart.isNull(HandMesh)) {
			end = start = getHandLocation(playerMesh);	
		}
		else {
			start = getBladeLocation(true);
			end = getBladeLocation(false);
		}
	}
	
	
	inline bool setItem(TObjectPtr<UObject> outer, USkeletalMeshComponent* const characterMesh, const UItem * item) {
		bool newlyCreated = HandMesh.setItem(outer, item);
		BladeStart.set("BladeStart", HandMesh);
		BladeEnd.set("BladeEnd", HandMesh);
		check(BladeStart.isNull(HandMesh) == BladeEnd.isNull(HandMesh));
		return newlyCreated;
	}
	void unset() {
		BladeStart.unset(HandMesh);
		BladeEnd.unset(HandMesh);
		HandMesh.unset();
	}
	
	inline void EnableWeaponTrace(bool EnableHandHitDetection, bool EnableFootHitDetection) {
		enableHandHitDetection |= EnableHandHitDetection;
		enableFootHitDetection |= EnableFootHitDetection;
	}
	
	void DisableWeaponTrace() {
		enableHandHitDetection = false;
		enableFootHitDetection = false;
	}
	

	bool HitDetectHand(AActor* owner, UWorld* world, USkeletalMeshComponent* const mesh, TArray<FHitResult>& OutHit);

	
};
