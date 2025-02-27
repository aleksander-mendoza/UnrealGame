// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatInventory.h"
#include "../item/ClothingItem.h"
#include "../item/WeaponItem.h"
#include "../ItemInstance.h"




void UCombatInventory::becomeArmed()
{
	//if (ArmedPoseType == EArmedPoseType::UNARMED) {
	const UWeaponAnims* mov = getCurrentMoveset();
	ArmedPoseType = mov->ArmedPose;
	const FWeaponAnim unsheathAnim = mov->Unsheath;
	if (unsheathAnim.Anim == nullptr) {
		unsheathBoth();
		EnableAttacking(GetAttackCooldown());
	}
	else {
		playAnim(&unsheathAnim);
	}
	//}
}

void UCombatInventory::becomeUnarmed()
{
	//if (ArmedPoseType != EArmedPoseType::UNARMED) {
	ArmedPoseType = EArmedPoseType::UNARMED;
	wantsToAttack = false;
	const FWeaponAnim& sheathAnim = getCurrentMoveset()->Sheath;
	if (sheathAnim.Anim == nullptr) {
		sheathBoth();
		EnableAttacking(0);
	}
	else {
		playAnim(&sheathAnim);
	}

	//}
}

void UCombatInventory::playAnim(const FWeaponAnim * anim)
{
	UAnimInstance* i = getAnimInstance();
	check(anim->Anim != nullptr);
	if (IsPlayingAttackAnim()) {
		stopAnim();
	}
	CurrentAttackAnim = anim;
	isAttackLeftHanded = wantsToAttackLeftHanded;
	i->Montage_Play(anim->Anim);
	DisableAttacking();
}

bool UCombatInventory::startAttack(bool leftHand)
{
	if (Health.CanAttack()) {
		wantsToAttack = true;
		wantsToAttackLeftHanded = leftHand;
		if (ArmedPoseType == EArmedPoseType::UNARMED) {
			becomeArmed();
		}
		else if (attackCooldown <= 0) {
			TObjectPtr<UItemInstance> item = getHandItem(leftHand);
			if (item) {
				item->startAttack(this, leftHand);
			}
			else {
				getCurrentMoveset()->startAttack(this, leftHand);
			}
			DisableAttacking();
			return true;
		}
	}
	return false;
}

void UCombatInventory::endAttack(bool leftHand)
{
	if (WantsToAttack(leftHand)) {
		TObjectPtr<UItemInstance> item = getHandItem(leftHand);
		if (item) {
			item->endAttack(this, leftHand);
		}
		else {
			getCurrentMoveset()->endAttack(this, leftHand);
		}
		wantsToAttack = false;
	}
}




void UCombatInventory::OnComboPartEnd()
{
	check(IsPlayingAttackAnim());
	if (!WantsToAttack() || isAttackLeftHanded != wantsToAttackLeftHanded) {
		float cooldown = GetAttackCooldown();
		if (IsPlayingAttackAnim()) {
			stopAnim(cooldown);
		}
		EnableAttacking(cooldown);
	}
}


bool UCombatInventory::onUnequipProjectile()
{
	return Super::onUnequipProjectile();
}

bool UCombatInventory::onUnequipDoubleHanded()
{
	if (Super::onUnequipDoubleHanded()) {
		//becomeUnarmed();
		return true;
	}
	return false;
}
bool UCombatInventory::onUnequipLeftHand()
{
	if (Super::onUnequipLeftHand()) {
		//becomeUnarmed();
		return true;
	}
	return false;
}

bool UCombatInventory::onUnequipRightHand()
{
	if (Super::onUnequipRightHand()) {
		//becomeUnarmed();
		return true;
	}
	return false;
}

bool UCombatInventory::onEquipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance> owner)
{
	return Super::onEquipProjectile(type, owner);
}

bool UCombatInventory::onEquipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::onEquipDoubleHanded(type, owner)) {
		bool wasSheathed = IsSheathed();
		sheathBoth();
		if (!wasSheathed) {
			becomeArmed();
		}
		return true;
	}
	return false;
}


bool UCombatInventory::onEquipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::onEquipLeftHand(type, owner)) {
		bool wasSheathed = IsSheathed();
		sheathBoth();
		if (!wasSheathed) {
			becomeArmed();
		}
		return true;
	}
	return false;
}

bool UCombatInventory::onEquipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::onEquipRightHand(type, owner)) {
		bool wasSheathed = IsSheathed();
		sheathBoth();
		if (!wasSheathed) {
			becomeArmed();
		}
		return true;
	}
	return false;
}

void UCombatInventory::stripHands()
{
	Super::stripHands();
	if (IsPlayingAttackAnim()) {
		stopAnim(ATTACK_STOP_BLENDOUT);
		EnableAttacking(ATTACK_STOP_BLENDOUT);
	}
}

bool UCombatInventory::TickCooldown(float DeltaTime)
{
	if (attackCooldown > 0) {
		attackCooldown -= DeltaTime;
		return false;
	}
	else {
		return WantsToAttack();
	}
}
void UCombatInventory::TickCombat(float DeltaTime)
{
	if (TickHealth(DeltaTime)) {
		TickInvincibility(DeltaTime);
		if (IsPlayingAttackAnim()) {
			TickHitDetection(DeltaTime);
		}
		else {
			if (TickCooldown(DeltaTime)) {
				startAttack();
			}
		}
	}
}
