// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyAttackCooldown.h"
#include "../GameCharacter.h"

void UNotifyAttackCooldown::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
//	OnComboNotify.Broadcast(IsLast);
	AGameCharacter * p = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (p) {
		p->NotifyAttackAnimationFinished(IsLeftHand);
	}
}