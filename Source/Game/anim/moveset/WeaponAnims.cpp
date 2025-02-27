// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAnims.h"
#include "../../items/inv/GameCharacterInventory.h"


bool UWeaponAnims::startAttack(UCombatInventory* user, bool leftHand) const
{
	const FWeaponAnim* anim = GetAnim();
	check(!user->IsPlayingAttackAnim());
	if (anim != nullptr) {
		user->CurrentAttackAnim = anim;
		user->playAnim(anim);
		return true;
	}
	return false;
}


void UWeaponAnims::endAttack(UCombatInventory* user, bool leftHand) const
{

}

void UWeaponAnims::cancelAttack(UCombatInventory* user) const
{
	
	if (user->IsPlayingAttackAnim()) {
		user->stopAnim();
	}
	else {
		user->becomeUnarmed();
	}
}
