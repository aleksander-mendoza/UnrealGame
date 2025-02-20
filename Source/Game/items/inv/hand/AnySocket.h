// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AnyMesh.h"
#include "AnySocket.generated.h"


USTRUCT(BlueprintType)
struct FAnySocket {
	GENERATED_BODY()

	UPROPERTY()
	const UStaticMeshSocket* ztatic;

	UPROPERTY()
	const USkeletalMeshSocket* skeletal;

	inline bool isNull(const FAnyMesh& meshComp) const {
		return meshComp.isSkeletal ? skeletal == nullptr : ztatic == nullptr;
	}
	inline FTransform GetSocketTransform(const FAnyMesh& meshComp) const {
		if (meshComp.isSkeletal) {
			return skeletal->GetSocketTransform(meshComp.skeletal);
		}
		else {
			FTransform trans;
			ztatic->GetSocketTransform(trans, meshComp.ztatic);
			return trans;
		}
	}
	void set(FName socketName, FAnyMesh& source);
	inline void unset(const FAnyMesh& source) {
		if (source.isSkeletal)skeletal = nullptr;
		else ztatic = nullptr;
	}

};