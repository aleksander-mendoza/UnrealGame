// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifySheath.h"
#include "../GameCharacter.h"

void UNotifySheath::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
//	OnComboNotify.Broadcast(IsLast);
	AGameCharacter * c = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (c) {
		UCombat* p = c->Combat;
		if (IsSheathLeft) {
			p->sheathLeft();
		}
		if (IsSheathRight) {
			p->sheathRight();
		}
		if (IsSheathBackRight) {
			p->unsheathBackRight();
		}
		if (IsSheathBackLeft) {
			p->unsheathBackLeft();
		}
		if (IsUnsheathLeft) {
			p->unsheathLeft();
		}
		if (IsUnsheathRight) {
			p->unsheathRight();
		}
	}
}