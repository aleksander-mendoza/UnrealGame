// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyWeaponTrace.h"
#include "../GameCharacter.h"


void UNotifyWeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AGameCharacter* p = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (p) {
		p->EnableWeaponTrace(enableLeftHandHitDetection, enableRightHandHitDetection, enableLeftFootHitDetection, enableRightFootHitDetection);
	}
}

void UNotifyWeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AGameCharacter* p = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (p) {
		p->DisableWeaponTrace();
	}
}
