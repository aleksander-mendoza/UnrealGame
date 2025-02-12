// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../items/ActorInventory.h"
#include "MovementSpeed.h"
#include "../anim/MeleeAttackClass.h"
#include "../anim/AttackHandedness.h"
#include "../anim/WeaponAnim.h"
#include "MovementEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../character/Combat.h"
#include "../character/Health.h"
#include "GameCharacterMovementComponent.generated.h"

#define ATTACK_DISABLED INFINITY
#define INVINCIBILITY_AFTER_HIT 0.2
#define ATTACK_STOP_BLENDOUT 0.2
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
	USkeletalMeshComponent* GetMesh() const;
private:
	float WalkSpeed = 0;

public:
	MovementEvents* listener;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCombat Combat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FHealth Health;

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
	bool bowShot = false;
	//FWeaponAnim * CurrentAttackAnim = nullptr;
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
	inline void sheath(bool unsheath, bool left, bool right) {
		USkeletalMeshComponent* mesh = GetMesh();
		if (unsheath) {
			if (left)Combat.Left.unsheath(mesh);
			if (right)Combat.Right.unsheath(mesh);
		}
		else {
			if (left)Combat.Left.sheath(mesh);
			if (right)Combat.Right.sheath(mesh);
		}
	}
	/*inline TObjectPtr<UAnimMontage>  GetCurrentAttackAnim() {
		if (CurrentAttackAnim == nullptr)return nullptr;
		check(CurrentAttackAnim->Anim != nullptr);
		return CurrentAttackAnim->Anim;
	}
	inline bool isPlayingAttackAnim() {
		return CurrentAttackAnim !=nullptr;
	}*/

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
		if (Health.CanRun()) {
			MovementSpeed = EMovementSpeed::RUN;
			MaxWalkSpeed = RunSpeed;
		}
	}
	inline void StartSlowWalking() {
		MovementSpeed = EMovementSpeed::SLOW_WALK;
		MaxWalkSpeed = SlowWalkSpeed;
	}
	inline bool IsBowShot() {
		return bowShot;
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

	FWeaponAnims * CurrentAttackAnims;
	FWeaponAnim * CurrentAttackAnim;
	inline bool isPlayingAttackAnim() const{
		return CurrentAttackAnim != nullptr;
	}
	inline bool AttackAnimRequiresMirroring() {
		if (CurrentAttackAnim != nullptr) {
			return CurrentAttackAnim->IsLeftHanded == isAttackLeftHanded;
		}
		return false;
	}
	inline void startMeleeAttack(EMeleeAttackClass weaponClass, UItemObject* weapon) {
		if (CurrentAttackAnims != nullptr && CurrentAttackAnim==nullptr) {
			CurrentAttackAnim = Combat.GetNextAnim(CurrentAttackAnims);
			if (CurrentAttackAnim != nullptr) {
				playAnim(*CurrentAttackAnim);
			}
		}
	}
	inline void becomeArmed(UActorInventory* inv) {
		ArmedPoseType = inv->getArmedPoseType();
		//inv->getHandItem(leftHand)
		const EMeleeAttackClass weaponClass = poseToAttack(ArmedPoseType);
		check(weaponClass != EMeleeAttackClass::NONE);
		CurrentAttackAnims = Combat.getAttackAnims(weaponClass);
		check(CurrentAttackAnims != nullptr);
		FWeaponAnim & unsheathAnim = CurrentAttackAnims->Unsheath;
		if (unsheathAnim.Anim == nullptr) {
			USkeletalMeshComponent * const mesh  = GetMesh();
			Combat.Left.unsheath(mesh);
			Combat.Right.unsheath(mesh);
			EnableAttacking(GetAttackCooldown());
			if (CurrentAttackAnim != nullptr) {
				check(CurrentAttackAnim->Anim != nullptr);
				getAnimInstance()->Montage_Stop(0, CurrentAttackAnim->Anim);
			}
			CurrentAttackAnim = nullptr;
		}
		else {
			playAnim(unsheathAnim);
		}
		//if (listener)listener->OnPoseBecameArmed(pose, item);
	}
	UAnimInstance* getAnimInstance();
	inline void playAnim(FWeaponAnim& anim) {
		UAnimInstance* i = getAnimInstance();
		check(anim.Anim != nullptr);
		if(CurrentAttackAnim != nullptr) {
			check(CurrentAttackAnim->Anim != nullptr);
			i->Montage_Stop(0, CurrentAttackAnim->Anim);
		}
		CurrentAttackAnim = &anim;
		isAttackLeftHanded = wantsToAttackLeftHanded;
		i->Montage_Play(anim.Anim);
		DisableAttacking();
	}
	inline void StopCurrentAttackAnim(float cooldown = 0) {
		if (CurrentAttackAnim) {
			check(CurrentAttackAnim->Anim != nullptr);
			EnableAttacking(cooldown);
			getAnimInstance()->Montage_Stop(ATTACK_STOP_BLENDOUT, CurrentAttackAnim->Anim);
			CurrentAttackAnim = nullptr;
			if (ArmedPoseType == EArmedPoseType::UNARMED) {
				CurrentAttackAnims = nullptr;
			}
		}
	}
	

	inline void attackStart(bool leftHand, UActorInventory * inv) {
		bowShot = false;
		wantsToAttack = true;
		wantsToAttackLeftHanded = leftHand;
		if (ArmedPoseType == EArmedPoseType::UNARMED) {
			becomeArmed(inv);
		}else if (attackCooldown <= 0 && (!listener || listener->AllowAttacking())) {
			/*check(!isPlayingAttackAnim());
			CurrentAttackAnim = nullptr;*/
			UItemObject* weapon = inv->getHandItem(leftHand);
			EItemClass weaponClass = weapon==nullptr?EItemClass::NONE : weapon->getItemClass();
			switch (weaponClass) {
			case EItemClass::BOW:
				if (leftHand) {
					ArmedPoseType = EArmedPoseType::BOW_AIMED;
					bowReadyToShoot = false;
					if (Combat.BowDrawArrow != nullptr) {
						getAnimInstance()->Montage_Play(Combat.BowDrawArrow);
					}
					DisableAttacking();
				}// TODO: right handed attack with bow allows for zoom-in
				//if (listener)listener->OnBowAimStarted(weapon);
				break;
			case EItemClass::DOUBLE_HANDED_WEAPON:
				if (!leftHand) {
					//if (listener)listener->OnBlockStarted(EMeleeAttackClass::DOUBLE_HANDED_WEAPON, weapon);
				}
				else {
					startMeleeAttack(EMeleeAttackClass::DOUBLE_HANDED_WEAPON, weapon);
				}
				break;
			case EItemClass::SINGLE_HANDED_QUIET_WEAPON:
			case EItemClass::SINGLE_HANDED_WEAPON:
				startMeleeAttack(EMeleeAttackClass::SINGLE_HANDED_WEAPON, weapon);
				break;
			default:
				startMeleeAttack(EMeleeAttackClass::BARE_HANDED, weapon);
				break;
			}
		}
	}
	void becomeUnarmed() {
		if (ArmedPoseType != EArmedPoseType::UNARMED) {
			ArmedPoseType = EArmedPoseType::UNARMED;
			wantsToAttack = false;
			bowShot = false;
			check(CurrentAttackAnims != nullptr);
			FWeaponAnim& sheathAnim = CurrentAttackAnims->Sheath;
			if (sheathAnim.Anim == nullptr) {
				USkeletalMeshComponent* const mesh = GetMesh();
				Combat.Left.sheath(mesh);
				Combat.Right.sheath(mesh);
				EnableAttacking(0);
				CurrentAttackAnim = nullptr;
			}
			else {
				playAnim(sheathAnim);
			}
			
		}
	}
	void attackEnd(bool leftHand) {
		if (wantsToAttack && wantsToAttackLeftHanded == leftHand) {
			attackEnd();
		}
	}
	void attackEnd();
	
	void attackCancel() {
		wantsToAttack = false;
		switch (ArmedPoseType) {
		case EArmedPoseType::UNARMED:
			break;
		case EArmedPoseType::BOW_AIMED:
			bowShot = false;
			EnableAttacking();
			ArmedPoseType = EArmedPoseType::BOW;
			break;
		default:
			if (isPlayingAttackAnim()) {
				StopCurrentAttackAnim();
			}
			else {
				becomeUnarmed();
			}
			break;
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
	bool bowReadyToShoot = false;
	void NotifyBowReadyToShoot() {
		bowReadyToShoot = true;
	}
	void NotifyAttackAnimationFinished() {
		Combat.NotifyAttackAnimationFinished();
		EnableAttacking(GetAttackCooldown());
		CurrentAttackAnim = nullptr;
		if (ArmedPoseType == EArmedPoseType::UNARMED) {
			CurrentAttackAnims = nullptr;
		}
	}
	void OnComboPartEnd() {
		check(isPlayingAttackAnim());
		if (!WantsToAttack() || isAttackLeftHanded!=wantsToAttackLeftHanded) {
			StopCurrentAttackAnim(GetAttackCooldown());
		}
	}
	void HitDetectHand(bool leftHand);
	void HitDetect() {
		HitDetectHand(false);
		HitDetectHand(true);
	}
	float invincibilityDuration = 0;
	void Hit(AGameCharacter* actor, UItemObject* weaponUsed, UItemObject* projectile, float damage);
	void Kill(AGameCharacter* killer) {
		Health.kill(GameCharacter, killer);
	}
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
