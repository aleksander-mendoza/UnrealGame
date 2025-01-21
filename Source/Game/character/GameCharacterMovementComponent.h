// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../items/ActorInventory.h"
#include "MovementSpeed.h"
#include "../anim/MeleeAttackClass.h"
#include "../anim/AttackHandedness.h"
#include "../anim/WeaponAnim.h"
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

	UPROPERTY(Category = "Character Movement: Attack", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float BowAttackCooldown = 0.2;

	UPROPERTY(Category = "Character Movement: Attack", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SingleHandedAttackCooldown = 0.2;

	UPROPERTY(Category = "Character Movement: Attack", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DoubleHandedAttackCooldown = 0.2;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SlowWalkSpeed = 100;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 500;
	
	EMovementSpeed MovementSpeed = EMovementSpeed::WALK;

	EArmedPoseType ArmedPoseType = EArmedPoseType::UNARMED;
	bool attackCancelled = false;
	FWeaponAnim * CurrentAttackAnim = nullptr;
	bool isAttackLeftHanded = false;
	class AGameCharacter * GameCharacter;
	bool wantsToAttack;
	bool wantsToAttackLeftHanded;
	float attackCooldown = 0;
	inline bool WantsToAttack() {
		return wantsToAttack;
	}
	inline bool WantsToAttackLeft() {
		return wantsToAttack && wantsToAttackLeftHanded;
	}
	inline bool WantsToAttackRight() {
		return wantsToAttack && !wantsToAttackLeftHanded;
	}

	inline TObjectPtr<UAnimMontage>  GetCurrentAttackAnim() {
		if (CurrentAttackAnim == nullptr)return nullptr;
		check(CurrentAttackAnim->Anim != nullptr);
		return CurrentAttackAnim->Anim;
	}
	inline bool isPlayingAttackAnim() {
		return CurrentAttackAnim !=nullptr;
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
		MaxWalkSpeed = WalkSpeed;
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
		MaxWalkSpeed = SlowWalkSpeed;
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
		attackCooldown = cooldown;
	}
	inline bool TickCooldown(float DeltaTime) {
		if (attackCooldown > 0) {
			attackCooldown -= DeltaTime;
			return false;
		}
		else {
			return WantsToAttack();
		}
	}
	inline void DisableAttacking() {
		EnableAttacking(ATTACK_DISABLED);
	}
	inline EMeleeAttackClass attackStart(bool leftHand, UActorInventory * inv) {
		attackCancelled = false;
		wantsToAttack = true;
		wantsToAttackLeftHanded = leftHand;
		if (ArmedPoseType == EArmedPoseType::UNARMED) {
			ArmedPoseType = inv->getArmedPoseType();
			return poseToAttack(ArmedPoseType);
		}else if (attackCooldown <= 0) {
			check(!isPlayingAttackAnim());
			CurrentAttackAnim = nullptr;
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
				return EMeleeAttackClass::DOUBLE_HANDED_WEAPON;
			case EItemClass::SINGLE_HANDED_QUIET_WEAPON:
			case EItemClass::SINGLE_HANDED_WEAPON:
				return EMeleeAttackClass::SINGLE_HANDED_WEAPON;
			default:
				return EMeleeAttackClass::BARE_HANDED;
			}
		}
		return EMeleeAttackClass::NONE;
	}
	UAnimInstance* getAnimInstance() const;
	void becomeUnarmed() {
		StopCurrentAttackAnim(0);
		ArmedPoseType = EArmedPoseType::UNARMED;
		wantsToAttack = false;
		attackCancelled = false;
	}
	void attackEnd(bool leftHand) {
		if (wantsToAttack && wantsToAttackLeftHanded == leftHand) {
			attackEnd();
		}
	}
	void attackEnd() {
		if (ArmedPoseType==EArmedPoseType::BOW_AIMED) {
			ArmedPoseType = EArmedPoseType::BOW;
			EnableAttacking(BowAttackCooldown);
		}
		wantsToAttack = false;
	}
	inline void StopCurrentAttackAnim(float cooldown=0) {
		//check(isPlayingAttackAnim());
		if (isPlayingAttackAnim()) {
			EnableAttacking(cooldown);
			getAnimInstance()->Montage_Stop(cooldown, GetCurrentAttackAnim());
			CurrentAttackAnim = nullptr;
		}
	}
	inline bool AttackAnimRequiresMirroring() {
		if (CurrentAttackAnim != nullptr) {
			return CurrentAttackAnim->IsLeftHanded == isAttackLeftHanded;
		}
		return false;
	}
	inline void PlayAttackAnim(bool leftHand, FWeaponAnim * anim) {
		check(anim != nullptr);
		check(anim->Anim != nullptr);
		//check(!isPlayingAttackAnim());
		UAnimInstance * i = getAnimInstance();
		if (CurrentAttackAnim != nullptr) {
			i->Montage_Stop(0, GetCurrentAttackAnim());
		}
		CurrentAttackAnim = anim;
		isAttackLeftHanded = leftHand;
		i->Montage_Play(GetCurrentAttackAnim());
		DisableAttacking();
		//}
	}
	EMeleeAttackClass attackCancel() {
		wantsToAttack = false;
		switch (ArmedPoseType) {
		case EArmedPoseType::UNARMED:
			return EMeleeAttackClass::NONE;
		case EArmedPoseType::BOW_AIMED:
			attackCancelled = true;
			EnableAttacking();
			ArmedPoseType = EArmedPoseType::BOW;
			return EMeleeAttackClass::NONE;
		default:
			if (isPlayingAttackAnim()) {
				attackCancelled = true;
				StopCurrentAttackAnim(GetAttackCooldown());
				return EMeleeAttackClass::NONE;
			}
			else {
				EMeleeAttackClass attackClass = poseToAttack(ArmedPoseType);
				ArmedPoseType = EArmedPoseType::UNARMED;
				return attackClass;
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
	void NotifyAttackAnimationFinished() {
		EnableAttacking(GetAttackCooldown());
		CurrentAttackAnim = nullptr;
	}
	void OnComboPartEnd() {
		check(isPlayingAttackAnim());
		if (!WantsToAttack() || isAttackLeftHanded!=wantsToAttackLeftHanded) {
			StopCurrentAttackAnim(GetAttackCooldown());
		}
	}
};
