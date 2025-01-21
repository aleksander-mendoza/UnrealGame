// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyCombo.h"
#include "../GameCharacter.h"

void UNotifyCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
//	OnComboNotify.Broadcast(IsLast);
	AGameCharacter * p = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (p)p->OnComboPartEnd();
}