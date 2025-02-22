// Fill out your copyright notice in the Description page of Project Settings.


#include "BowAnims.h"
#include "../../GameCharacter.h"

bool UBowAnims::startAttack(UGameCharacterInventory* user, bool leftHand) const
{
	if (leftHand) {
		user->BowShot = false;
		user->setArmedStatus(EArmedPoseType::BOW_AIMED);
		user->bowReadyToShoot = false;
		if (BowDrawArrow != nullptr) {
			user->getAnimInstance()->Montage_Play(BowDrawArrow);
		}
		user->DisableAttacking();
		return true;
	}
	return false;
}

void UBowAnims::endAttack(UGameCharacterInventory* user, bool leftHand) const
{
	if (user->ArmedPoseType == EArmedPoseType::BOW_AIMED) {
		user->setArmedStatus(EArmedPoseType::BOW);
		if (user->bowReadyToShoot) {
			if (BowShootArrow != nullptr) {
				user->getAnimInstance()->Montage_Play(BowShootArrow);
			}
			user->shootProjectile(1000);
		}
		user->BowShot = true;
		user->EnableAttacking(user->Health.BowAttackCooldown);
	}
	user->wantsToAttack = false;
}

void UBowAnims::cancelAttack(UGameCharacterInventory* user) const
{
	
	if (user->ArmedPoseType == EArmedPoseType::BOW_AIMED) {
		user->BowShot = false;
		user->EnableAttacking();
		user->ArmedPoseType = EArmedPoseType::BOW;
	}
	else {
		user->becomeUnarmed();
	}
}
