// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldEntityInventory.h"
#include "CombatInventory.generated.h"

#define ATTACK_DISABLED INFINITY
#define ATTACK_STOP_BLENDOUT 0.2
//DECLARE_DELEGATE_OneParam(FStringDelegate, FString);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UCombatInventory : public UWorldEntityInventory
{
	GENERATED_BODY()

	

public:	
	
	EArmedPoseType ArmedPoseType = EArmedPoseType::UNARMED;
	bool BowShot = false;
	bool isAttackLeftHanded = false;
	bool wantsToAttack = false;
	bool wantsToAttackLeftHanded = false;
	float attackCooldown = 0;
	const FWeaponAnim* CurrentAttackAnim;

	////////////////////////////////////////
	/////////////// GETTERS ////////////////
	////////////////////////////////////////
	inline bool WantsToAttack() {
		return wantsToAttack;
	}
	inline bool WantsToAttack(bool leftHand) {
		return wantsToAttack && wantsToAttackLeftHanded == leftHand;
	}
	inline bool WantsToAttackLeft() {
		return WantsToAttack(true);
	}
	inline bool WantsToAttackRight() {
		return WantsToAttack(false);
	}
	inline EArmedPoseType getArmedStatus() {
		return ArmedPoseType;
	}
	inline void setArmedStatus(EArmedPoseType pose) {
		ArmedPoseType = pose;
	}
	inline bool IsPlayingAttackAnim() const {
		return CurrentAttackAnim != nullptr;
	}
	inline bool AttackAnimRequiresMirroring() {
		if (CurrentAttackAnim != nullptr) {
			return CurrentAttackAnim->IsLeftHanded == isAttackLeftHanded;
		}
		return false;
	}
	inline void DisableAttacking() {
		EnableAttacking(ATTACK_DISABLED);
	}
	inline bool IsBowShot() {
		return BowShot;
	}
	inline bool AimsBow() {
		return ArmedPoseType == EArmedPoseType::BOW_AIMED;
	}
	inline bool CanJump() {
		return !AimsBow() && Health.CanJump();
	}
	inline bool CanRun() {
		return !AimsBow() && Health.CanRun();
	}
	inline float GetAttackCooldown() {
		return Health.GetAttackCooldown(getArmedStatus());
	}
	////////////////////////////////////////
	/////////////// ATTACKS ////////////////
	////////////////////////////////////////

	void becomeArmed();
	void becomeUnarmed();
	void playAnim(const FWeaponAnim * anim);
	inline void stopAnim(float blendout = ATTACK_STOP_BLENDOUT) {
		check(IsPlayingAttackAnim());
		check(CurrentAttackAnim->Anim != nullptr);
		getAnimInstance()->Montage_Stop(blendout, CurrentAttackAnim->Anim);
		CurrentAttackAnim = nullptr;
	}
	bool startAttack() {
		return startAttack(wantsToAttackLeftHanded);
	}
	bool startAttack(bool leftHand);
	void endAttack(bool leftHand);


	inline void cancelAttack() {
		wantsToAttack = false;
		getCurrentMoveset()->cancelAttack(this);
	}
	inline void EnableAttacking(float cooldown = 0) {
		attackCooldown = cooldown;
	}
	bool bowReadyToShoot = false;
	void NotifyBowReadyToShoot() {
		bowReadyToShoot = true;
	}
	virtual void NotifyAttackAnimationFinished() override {
		Super::NotifyAttackAnimationFinished();
		EnableAttacking();
		CurrentAttackAnim = nullptr;
	}
	void OnComboPartEnd();
	
	

	////////////////////////////////////////
	/////////////// EVENTS  ////////////////
	////////////////////////////////////////
	
	virtual bool onUnequipProjectile() override;

	virtual bool onUnequipDoubleHanded() override;

	virtual bool onUnequipLeftHand() override;

	virtual bool onUnequipRightHand() override;

	virtual bool onEquipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual bool onEquipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual bool onEquipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual bool onEquipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) override;
	
	virtual void stripHands() override;

	////////////////////////////////////////
	/////////////// TICKS   ////////////////
	////////////////////////////////////////
	
	bool TickCooldown(float DeltaTime);

	void TickCombat(float DeltaTime);

};
