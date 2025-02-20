// Fill out your copyright notice in the Description page of Project Settings.


#include "AnyMesh.h"

UPrimitiveComponent* FAnyMesh::findSocket(FVector& location, FRotator& rotation, const FName itemSocket){
	if (isSkeletal) {
		if (skeletal != nullptr) {
			USkeletalMesh* const itemMesh = skeletal->SkeletalMesh;
			if (itemMesh) {
				const USkeletalMeshSocket* const socket = itemMesh->FindSocket(itemSocket);
				if (socket) {
					location = socket->RelativeLocation;
					rotation = socket->RelativeRotation;
				}
			}
		}
		return skeletal;
	}
	else {
		if (ztatic != nullptr) {
			UStaticMesh* const itemMesh = ztatic->GetStaticMesh();
			if (itemMesh) {
				const UStaticMeshSocket* const socket = itemMesh->FindSocket(itemSocket);
				if (socket) {
					location = socket->RelativeLocation;
					rotation = socket->RelativeRotation;
				}
			}
		}
		return ztatic;
	}
}