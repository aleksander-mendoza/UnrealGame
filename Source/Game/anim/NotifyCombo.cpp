// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyCombo.h"


void UNotifyCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	if (Owner != nullptr) {
		Owner->OnComboPartEnd(IsLast);
	}

}