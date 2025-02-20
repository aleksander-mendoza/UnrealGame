// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacterInventory.h"
#include "../item/ClothingItem.h"
#include "../item/WeaponItem.h"
#include "../ItemInstance.h"




void UGameCharacterInventory::becomeArmed()
{
	if (ArmedPoseType == EArmedPoseType::UNARMED) {
		const UWeaponAnims* mov = getCurrentMoveset();
		ArmedPoseType = mov->ArmedPose;
		if (IsPlayingAttackAnim()) {
			stopAnim();
		}
		const FWeaponAnim unsheathAnim = mov->Unsheath;
		if (unsheathAnim.Anim == nullptr) {
			unsheathBoth();
			EnableAttacking(GetAttackCooldown());
		}
		else {
			playAnim(&unsheathAnim);
		}
	}
}

void UGameCharacterInventory::becomeUnarmed()
{
	if (ArmedPoseType != EArmedPoseType::UNARMED) {
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

	}
}

void UGameCharacterInventory::playAnim(const FWeaponAnim * anim)
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

bool UGameCharacterInventory::startAttack(bool leftHand)
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

void UGameCharacterInventory::endAttack(bool leftHand)
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




void UGameCharacterInventory::OnComboPartEnd()
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


bool UGameCharacterInventory::unequipProjectile()
{
	return Super::unequipProjectile();
}

bool UGameCharacterInventory::unequipDoubleHanded()
{
	if (Super::unequipDoubleHanded()) {
		//becomeUnarmed();
		return true;
	}
	return false;
}
bool UGameCharacterInventory::unequipLeftHand()
{
	if (Super::unequipLeftHand()) {
		//becomeUnarmed();
		return true;
	}
	return false;
}

bool UGameCharacterInventory::unequipRightHand()
{
	if (Super::unequipRightHand()) {
		//becomeUnarmed();
		return true;
	}
	return false;
}

bool UGameCharacterInventory::equipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance> owner)
{
	return Super::equipProjectile(type, owner);
}

bool UGameCharacterInventory::equipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::equipDoubleHanded(type, owner)) {
		bool wasSheathed = IsSheathed();
		sheathBoth();
		if (!wasSheathed) {
			becomeArmed();
		}
		return true;
	}
	return false;
}


bool UGameCharacterInventory::equipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::equipLeftHand(type, owner)) {
		bool wasSheathed = IsSheathed();
		sheathBoth();
		if (!wasSheathed) {
			becomeArmed();
		}
		return true;
	}
	return false;
}

bool UGameCharacterInventory::equipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::equipRightHand(type, owner)) {
		bool wasSheathed = IsSheathed();
		sheathBoth();
		if (!wasSheathed) {
			becomeArmed();
		}
		return true;
	}
	return false;
}

bool UGameCharacterInventory::TickCooldown(float DeltaTime)
{
	if (attackCooldown > 0) {
		attackCooldown -= DeltaTime;
		return false;
	}
	else {
		return WantsToAttack();
	}
}
void UGameCharacterInventory::TickEverything(float DeltaTime)
{
	TickHealth(DeltaTime);
	if (IsPlayingAttackAnim()) {
		TickHitDetection(DeltaTime);
	}
	else {
		if (TickCooldown(DeltaTime)) {
			startAttack();
		}
	}
}
