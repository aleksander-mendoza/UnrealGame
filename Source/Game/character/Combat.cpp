// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat.h"
#include "../GameCharacter.h"

UCombat::UCombat() {
	LeftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHandMesh"));
	RightHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHandMesh"));
	LeftHandMesh->SetSimulatePhysics(false);
	RightHandMesh->SetSimulatePhysics(false);
	LeftHandMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RightHandMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RightHandMesh->RegisterComponent();
	LeftHandMesh->RegisterComponent();

	HandSocketR = TEXT("hand_rSocket");
	HandSocketL = TEXT("hand_lSocket");
	SheathedSocketL = TEXT("sheathed_lSocket");
	SheathedSocketR = TEXT("sheathed_rSocket");
	SheathedSocketBackL = TEXT("sheathed_back_lSocket");
	SheathedSocketBackR = TEXT("sheathed_back_rSocket");
}

void UCombat::BeginPlay()
{
	USkeletalMeshComponent* Mesh = GetMesh();
	BareHandSocket[true] = Mesh->GetSocketByName(HandSocketL);
	BareHandSocket[false] = Mesh->GetSocketByName(HandSocketR);
}

USkeletalMeshComponent* UCombat::GetMesh() const
{
	return GameCharacter->GetMesh();
}

