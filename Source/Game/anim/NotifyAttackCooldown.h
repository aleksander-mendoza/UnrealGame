// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "../GameCharacter.h"
#include "NotifyAttackCooldown.generated.h"

//DECLARE_MULTICAST_DELEGATE_OneParam(FOnComboNotifySignature, bool);
/**
 * 
 */
UCLASS()
class GAME_API UNotifyAttackCooldown : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsLeftHand = false;
};
