// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacterInventory.h"


void UGameCharacterInventory::setHairdo(int index)
{
	const FGender& gender = GetGender();
	HairdoIndex = FMath::Min(index, gender.Hairdos.Num() - 1);
	USkeletalMesh* mesh = HairdoIndex < 0 ? nullptr : gender.Hairdos[HairdoIndex].getSkeletalMesh();
	USkeletalMeshComponent * playerMesh =  getPlayerMesh();
	if (mesh == nullptr || playerMesh==nullptr) {
		if (HairMeshComponent != nullptr)HairMeshComponent->SetSkeletalMesh(nullptr);
		return;
	}
	if (HairMeshComponent == nullptr) {

		check(getPlayerMesh() != nullptr);
		HairMeshComponent = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass());
		HairMeshComponent->RegisterComponent();
		HairMeshComponent->AttachToComponent(playerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, HairSocket);
		
		HairMeshComponent->SetAnimInstanceClass(playerMesh->AnimClass);
		HairMeshComponent->SetLeaderPoseComponent(playerMesh, true);
		
	}
	HairMeshComponent->SetSkeletalMesh(mesh);
	HairMeshComponent->SetSimulatePhysics(gender.Hairdos[HairdoIndex].SimulatePhysics);

	if (DynamicHairMaterial == nullptr) {
		FSkeletalMaterial& mat = mesh->GetMaterials()[0];
		DynamicHairMaterial = UMaterialInstanceDynamic::Create(mat.MaterialInterface, this);
		DynamicHairMaterial->SetVectorParameterValue("color", HairColor);
	}
	HairMeshComponent->SetMaterial(0, DynamicHairMaterial);
	
	
	
}

void UGameCharacterInventory::setPlayerMesh(USkeletalMeshComponent* playerMesh)
{
	Super::setPlayerMesh(playerMesh);
	if (Race != nullptr) {
		playerMesh->SetSkeletalMesh(Race->getBodyMesh(IsFemale()));
		playerMesh->SetAnimInstanceClass(Race->getAnimClass(IsFemale()));
	}
	if (HairMeshComponent != nullptr) {
		HairMeshComponent->SetAnimInstanceClass(playerMesh->AnimClass);
		HairMeshComponent->SetLeaderPoseComponent(playerMesh, true);
	}
}
