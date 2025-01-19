// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../items/ActorInventory.h"
#include "MovementSpeed.h"
#include "../anim/MeleeAttackClass.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameCharacterMovementComponent.generated.h"

#define ATTACK_DISABLED INFINITY

/**
 * 
 */
UCLASS()
class GAME_API UGameCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	UGameCharacterMovementComponent();

	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
private:
	float WalkSpeed = 0;

public:

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float BowAttackCooldown = 0.2;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SingleHandedAttackCooldown = 0.2;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DoubleHandedAttackCooldown = 0.2;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SlowWalkSpeed = 100;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 500;
	
	EMovementSpeed MovementSpeed = EMovementSpeed::WALK;

	EArmedPoseType ArmedPoseType = EArmedPoseType::UNARMED;
	bool attackCancelled = false;
	TObjectPtr<UAnimMontage> CurrentAttackComboAnim[2] = { nullptr, nullptr };
	class AGameCharacter * GameCharacter;
	bool wantsToAttack[2] = { false, false };
	float attackCooldown[2] = { 0,0 };
	inline bool WantsToAttack(bool leftHand) {
		return wantsToAttack[leftHand];
	}
	inline bool wantsToAttackLeft() {
		return WantsToAttack(true);
	}
	inline bool wantsToAttackRight() {
		return WantsToAttack(false);
	}

	inline TObjectPtr<UAnimMontage>  GetCurrentAttackAnim(bool leftHand) {
		
		return CurrentAttackComboAnim[leftHand];
	}
	inline TObjectPtr<UAnimMontage>  leftAttackAnim() {
		return GetCurrentAttackAnim(true);
	}
	inline TObjectPtr<UAnimMontage>  rightAttackAnim() {
		return GetCurrentAttackAnim(false);
	}
	inline bool isPlayingAttackAnim() {
		return leftAttackAnim() != nullptr || rightAttackAnim() != nullptr;
	}
	inline bool isPlayingAttackAnim(bool leftHand) {
		return GetCurrentAttackAnim(leftHand) != nullptr;
	}

	inline EArmedPoseType getArmedStatus() {
		return ArmedPoseType;
	}
	inline void setArmedStatus(EArmedPoseType a) {
		ArmedPoseType = a;
	}

	void ToggleSlowWalking()
	{
		if (IsSlowWalking()) {
			StartWalking();
		}
		else {
			StartSlowWalking();
		}
	}
	inline bool AimsBow() {
		return ArmedPoseType == EArmedPoseType::BOW_AIMED;
	}
	inline bool CanJump() {
		return !AimsBow();
	}
	inline void StartWalking() {
		MovementSpeed = EMovementSpeed::WALK;
		MaxWalkSpeed = RunSpeed;
	}
	inline bool CanStartRunning() {
		return !AimsBow();
	}
	inline void StartRunning() {
		if (CanStartRunning()) {
			MovementSpeed = EMovementSpeed::RUN;
			MaxWalkSpeed = RunSpeed;
		}
	}
	inline void StartSlowWalking() {
		MovementSpeed = EMovementSpeed::SLOW_WALK;
		MaxWalkSpeed = RunSpeed;
	}
	inline bool IsAttackCancelled() {
		return attackCancelled;
	}
	inline bool IsRunning() {
		return IsWalking() && MovementSpeed == EMovementSpeed::RUN;
	}
	inline bool IsSlowWalking() {
		return IsWalking() && MovementSpeed == EMovementSpeed::SLOW_WALK;
	}

	inline bool IsDirectionalMovement() {
		return bOrientRotationToMovement;
	}
	inline void ToggleDirectionalMovement(bool trueDirectionalMovement) {
		bOrientRotationToMovement = trueDirectionalMovement;
		bUseControllerDesiredRotation = !trueDirectionalMovement;
	}
	inline void Move(const FVector & MovementVector, const FRotator & Rotation) {
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if (IsSwimming()) {
			// add movement 
			const FVector UpDirection(0, 0, 1);
			AddInputVector(UpDirection * MovementVector.Z);
		}
		AddInputVector(ForwardDirection * MovementVector.Y);
		AddInputVector(RightDirection * MovementVector.X);
	}
	inline void EnableAttacking(float cooldown = 0) {
		EnableAttacking(false, cooldown);
		EnableAttacking(true, cooldown);
	}
	inline bool TickCooldown(bool leftHand, float DeltaTime) {
		if (attackCooldown[leftHand] > 0) {
			attackCooldown[leftHand] -= DeltaTime;
			return false;
		}
		else {
			return wantsToAttack[leftHand];
		}
	}
	inline void EnableAttacking(bool leftHand, float cooldown=0) {
		attackCooldown[leftHand] = cooldown;
	}
	inline void DisableAttacking(bool leftHand) {
		attackCooldown[leftHand] = ATTACK_DISABLED;
	}
	inline void DisableAttacking() {
		DisableAttacking(true);
		DisableAttacking(false);
	}
	inline EMeleeAttackClass attackStart(bool leftHand, UActorInventory * inv) {
		attackCancelled = false;
		wantsToAttack[leftHand] = true;
		if (ArmedPoseType == EArmedPoseType::UNARMED) {
			ArmedPoseType = inv->getArmedPoseType();
			DisableAttacking(); // unsheathing animation starts. 
			// The animation is expected to notify us at the end and reset cooldown to 0
			return EMeleeAttackClass::NONE;
		}else if (attackCooldown[leftHand] <= 0) {
			check(CurrentAttackComboAnim[leftHand] == nullptr);
			CurrentAttackComboAnim[leftHand] = nullptr;
			EItemClass weaponClass = inv->getWeaponClass(leftHand);
			switch (weaponClass) {
			case EItemClass::BOW:
				if (leftHand) {
					ArmedPoseType = EArmedPoseType::BOW_AIMED;
				}// TODO: right handed attack with bow allows for zoom-in
				return EMeleeAttackClass::NONE;
			case EItemClass::DOUBLE_HANDED_WEAPON:
				if (!leftHand) {
					// TODO: right handed attack with double handed weapon works like blocking
					return EMeleeAttackClass::NONE;
				}
				DisableAttacking(leftHand);
				return EMeleeAttackClass::DOUBLE_HANDED_WEAPON;
			case EItemClass::SINGLE_HANDED_QUIET_WEAPON:
			case EItemClass::SINGLE_HANDED_WEAPON:
				DisableAttacking(leftHand);
				return EMeleeAttackClass::SINGLE_HANDED_WEAPON;
			default:
				DisableAttacking(leftHand);
				return EMeleeAttackClass::BARE_HANDED;
			}
		}
		return EMeleeAttackClass::NONE;
	}
	UAnimInstance* getAnimInstance() const;

	void attackEnd(bool leftHanded) {
		if (ArmedPoseType==EArmedPoseType::BOW_AIMED) {
			ArmedPoseType = EArmedPoseType::BOW;
			EnableAttacking(BowAttackCooldown);
		}
		wantsToAttack[leftHanded] = false;
	}
	inline void StopCurrentAttackAnim(float cooldown=0) {
		StopCurrentAttackAnim(true, cooldown);
		StopCurrentAttackAnim(false, cooldown);
	}
	inline void StopCurrentAttackAnim(bool leftHand, float cooldown=0) {
		if (isPlayingAttackAnim(leftHand)) {
			EnableAttacking(leftHand, cooldown);
			getAnimInstance()->Montage_Stop(GetAttackCooldown(), GetCurrentAttackAnim(leftHand));
			CurrentAttackComboAnim[leftHand] = nullptr;
		}
	}
	inline void PlayAttackAnim(bool leftHand, TObjectPtr<UAnimMontage> anim) {
		CurrentAttackComboAnim[leftHand] = anim;
		getAnimInstance()->Montage_Play(anim);
	}
	bool attackCancel() {
		wantsToAttack[0] = false;
		wantsToAttack[1] = false;
		switch (ArmedPoseType) {
		case EArmedPoseType::UNARMED:
			return false;
		case EArmedPoseType::BOW_AIMED:
			attackCancelled = true;
			EnableAttacking();
			ArmedPoseType = EArmedPoseType::BOW;
			return false;
		default:
			if (isPlayingAttackAnim()) {
				attackCancelled = true;
				StopCurrentAttackAnim(GetAttackCooldown());
				return true;
			}
			else {
				ArmedPoseType = EArmedPoseType::UNARMED;
				return false;
			}
		}
	}
	float GetAttackCooldown() {
		switch(ArmedPoseType){
		case EArmedPoseType::DOUBLE_HANDED:
			return DoubleHandedAttackCooldown;
		case EArmedPoseType::BARE_HANDED:
		case EArmedPoseType::SINGLE_HANDED:
		case EArmedPoseType::UNARMED:
			return SingleHandedAttackCooldown;
		case EArmedPoseType::BOW_AIMED:
		case EArmedPoseType::BOW:
			return BowAttackCooldown;
		default:
			check(false);
			return SingleHandedAttackCooldown;
		}
	}
	void NotifyAttackAnimationFinished(bool leftHand) {
		CurrentAttackComboAnim[leftHand] = nullptr;
		EnableAttacking(leftHand, GetAttackCooldown());
	}
	void OnComboPartEnd(bool leftHand) {
		if (isPlayingAttackAnim(leftHand) && !WantsToAttack(leftHand)) {
			StopCurrentAttackAnim(leftHand, GetAttackCooldown());
		}
	}
};
