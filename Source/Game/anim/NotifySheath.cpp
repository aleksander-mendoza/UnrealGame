// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifySheath.h"
#include "../GameCharacter.h"

void UNotifySheath::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
//	OnComboNotify.Broadcast(IsLast);
	AGameCharacter * c = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (c) {
		c->GameMovement->sheath(IsUnsheath, IsLeft, IsRight);
		
	}
}