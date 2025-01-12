// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyWeaponTrace.h"
#include "../GameCharacter.h"


void UNotifyWeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AGameCharacter* p = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (p) {
		p->enableLeftHandHitDetection |= enableLeftHandHitDetection;
		p->enableRightHandHitDetection |= enableRightHandHitDetection;
		p->enableLeftFootHitDetection |= enableLeftFootHitDetection;
		p->enableRightFootHitDetection |= enableRightFootHitDetection;
		p->hitDetectionTimer = 0;
	}
}

void UNotifyWeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AGameCharacter* p = Cast<AGameCharacter>(MeshComp->GetOwner());
	if (p) {
		if (enableLeftHandHitDetection)p->enableLeftHandHitDetection = false;
		if (enableRightHandHitDetection)p->enableRightHandHitDetection = false;
		if (enableLeftFootHitDetection)p->enableLeftFootHitDetection = false;
		if (enableRightFootHitDetection)p->enableRightFootHitDetection = false;
	}
}
