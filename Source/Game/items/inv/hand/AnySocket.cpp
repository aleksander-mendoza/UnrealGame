// Fill out your copyright notice in the Description page of Project Settings.


#include "AnySocket.h"

void FAnySocket::set(FName socketName, FAnyMesh& source) {
	if (source.isSkeletal) {
		USkeletalMesh* const itemMesh = source.skeletal->SkeletalMesh;
		if (itemMesh) {
			skeletal = itemMesh->FindSocket(socketName);
		}
	}
	else {
		UStaticMesh* const itemMesh = source.ztatic->GetStaticMesh();
		if (itemMesh) {
			const UStaticMeshSocket* const socket = itemMesh->FindSocket(socketName);
		}
	}
}