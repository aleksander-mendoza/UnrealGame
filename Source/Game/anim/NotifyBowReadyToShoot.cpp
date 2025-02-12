// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyBowReadyToShoot.h"
#include "../GameCharacter.h"

void UNotifyBowReadyToShoot::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
//	OnComboNotify.Broadcast(IsLast);
	AGameCharacter * p = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (p) {
		p->NotifyBowReadyToShoot();
	}
}