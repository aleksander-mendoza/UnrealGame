// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../../item/Item.h"
#include "AnyMesh.generated.h"



USTRUCT(BlueprintType)
struct FAnyMesh {
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> ztatic;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> skeletal;
	
	bool isSkeletal;

	inline UPrimitiveComponent * getComponent() {
		return isSkeletal ? (UPrimitiveComponent *) skeletal : ztatic;
	}
	inline bool create(TObjectPtr<UObject> outer) {
		if (isSkeletal) {
			if (skeletal == nullptr) {
				skeletal = NewObject<USkeletalMeshComponent>(outer, USkeletalMeshComponent::StaticClass());
				skeletal->SetSimulatePhysics(false);
				skeletal->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
				skeletal->RegisterComponent();
				return true;
			}
		}
		else {
			if (ztatic == nullptr) {
				ztatic = NewObject<UStaticMeshComponent>(outer, UStaticMeshComponent::StaticClass());
				ztatic->SetSimulatePhysics(false);
				ztatic->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
				ztatic->RegisterComponent();
				return true;
			}
		}
		return false;
	}
	inline void unset() {
		if (isSkeletal) {
			if (skeletal)skeletal->SetSkeletalMesh(nullptr);
		}
		else {
			if (ztatic)ztatic->SetStaticMesh(nullptr);
		}
	}
	inline bool setItem(TObjectPtr<UObject> outer, const UItem * item) {
		isSkeletal = item->isSkeletal();
		bool wasNewlyCreated = create(outer);
		if (isSkeletal) {
			skeletal->SetSkeletalMesh(item->getSkeletalMesh());
		}
		else {
			ztatic->SetStaticMesh(item->getMesh());
		}
		return wasNewlyCreated;
	}
	
	UPrimitiveComponent* findSocket(FVector& location, FRotator& rotation, const FName itemSocket);
	inline void setTransform(FVector location, FRotator rotation) {
		UPrimitiveComponent* c = getComponent();
		c->SetRelativeLocation(location);
		c->SetRelativeRotation(rotation);
	}
	inline void attach(USkeletalMeshComponent* const characterMesh, const FName& itemSocket, const FName characterSocket) {
		FVector location;
		FRotator rotation;
		UPrimitiveComponent* comp = findSocket(location, rotation, itemSocket);
		if (comp != nullptr) {
			const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
			comp->SetRelativeLocation(location);
			comp->SetRelativeRotation(rotation);
			comp->AttachToComponent(characterMesh, atr, characterSocket);
		}
	}
	
};
