// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/SkeletalMeshSocket.h"
#include "../anim/WeaponSetAnims.h"
#include "../items/ActorInventory.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Combat.generated.h"

#define HIT_DETECTION_PERIOD 0.1

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAME_API UCombat : public UActorComponent
{
	GENERATED_BODY()

	UCombat();


	virtual void BeginPlay() override;
public:

	unsigned int comboAnimIdx = 0;
	class AGameCharacter* GameCharacter;
	/** Death Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> DeathAnims;

	/** Hit Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> HitAnims;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
	FWeaponSetAnims WeaponAnims;

	/** Hand socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FName HandSocketR;
	/** Hand socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FName HandSocketL;
	/** Sheathed weapon socket (left)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocketL;
	/** Sheathed weapon socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocketR;
	/** Sheathed weapon socket (back left)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocketBackL;

	/** Sheathed weapon socket (back right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocketBackR;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> LeftHandMesh;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> RightHandMesh;

	inline TObjectPtr<UStaticMeshComponent>  ItemMesh(bool leftHand) {
		return leftHand ? LeftHandMesh : RightHandMesh;
	}

	const UStaticMeshSocket* BladeStart[2];
	const UStaticMeshSocket* BladeEnd[2];
	const USkeletalMeshSocket* BareHandSocket[2];

	bool enableHandHitDetection[2];
	bool enableFootHitDetection[2];
	inline bool HasWeaponEquipped(bool leftHand) {
		return BladeStart[leftHand]!=nullptr;
	}
	inline const USkeletalMeshSocket* GetHandSocket(bool leftHand) {
		return BareHandSocket[leftHand];
	}
	inline const UStaticMeshSocket* GetBladeStart(bool leftHand) {
		return BladeStart[leftHand];
	}
	inline const UStaticMeshSocket* GetBladeEnd(bool leftHand) {
		return BladeEnd[leftHand];
	}
	inline FVector GetHandLocation(bool leftHand) {
		return GetHandSocket(leftHand)->GetSocketLocation(GetMesh());
	}
	inline FVector GetBladeStartLocation(bool leftHand) {
		FTransform trans;
		GetBladeStart(leftHand)->GetSocketTransform(trans, ItemMesh(leftHand));
		return trans.GetLocation();
	}
	inline FVector GetBladeEndLocation(bool leftHand) {
		FTransform trans;
		GetBladeEnd(leftHand)->GetSocketTransform(trans, ItemMesh(leftHand));
		return trans.GetLocation();
	}
	inline void GetStartEndLocation(bool leftHand, FVector & start, FVector& end) {
		if (HasWeaponEquipped(leftHand)) {
			start = GetBladeStartLocation(leftHand);
			end = GetBladeEndLocation(leftHand);
		}
		else {
			end = start = GetHandLocation(leftHand);
		}
	}
	inline void EnableHandHitDetection(bool leftHand, bool enabled=true) {
		enableHandHitDetection[leftHand] = enabled;
	}
	inline void EnableFootHitDetection(bool leftFoot, bool enabled = true) {
		enableFootHitDetection[leftFoot] = true;
	}
	inline bool IsEnabledHandHitDetection(bool leftHand) {
		return enableHandHitDetection[leftHand];
	}
	inline bool IsEnabledRightHandHitDetection() {
		return IsEnabledHandHitDetection(false);
	}
	inline bool IsEnabledLeftHandHitDetection() {
		return IsEnabledHandHitDetection(true);
	}
	
	float hitDetectionTimer;
	
	USkeletalMeshComponent* GetMesh() const;

	
	inline void sheathLeft() {
		const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
		LeftHandMesh->AttachToComponent(GetMesh(), atr, SheathedSocketL);
	}
	inline void sheathRight() {
		const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
		RightHandMesh->AttachToComponent(GetMesh(), atr, SheathedSocketR);
	}
	inline void unsheathBackRight() {
		const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
		RightHandMesh->AttachToComponent(GetMesh(), atr, SheathedSocketBackR);
	}
	inline void unsheathBackLeft() {
		const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
		LeftHandMesh->AttachToComponent(GetMesh(), atr, SheathedSocketBackL);
	}
	inline void unsheathLeft() {
		const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
		LeftHandMesh->AttachToComponent(GetMesh(), atr, HandSocketL);
	}
	inline void unsheathRight() {
		const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
		RightHandMesh->AttachToComponent(GetMesh(), atr, HandSocketR);
	}
	inline TObjectPtr<UAnimMontage> getHitAnim() {
		return HitAnims.Num()==0?nullptr: HitAnims[comboAnimIdx++ % HitAnims.Num()];
	}
	inline TObjectPtr<UAnimMontage> getDeathAnim() {
		return DeathAnims.Num() == 0 ? nullptr : DeathAnims[comboAnimIdx++ % DeathAnims.Num()];
	}
	inline TObjectPtr<UAnimMontage> getAttackAnim(EMeleeAttackClass weaponClass, bool leftHand) {
		return WeaponAnims.getAttackAnim(weaponClass, leftHand, comboAnimIdx++);
	}
	inline TObjectPtr<UAnimMontage> getLeftAttackAnim(EMeleeAttackClass weaponClass) {
		return getAttackAnim(weaponClass, true);
	}
	inline TObjectPtr<UAnimMontage> getRightAttackAnim(EMeleeAttackClass weaponClass) {
		return getAttackAnim(weaponClass, false);
	}
	inline void EquipItem(bool leftHand, TObjectPtr < UItemObject > item) {
		EquipItem(leftHand, item->getMesh());
	}
	inline void EquipItem(bool leftHand, UStaticMesh* m) {
		ItemMesh(leftHand)->SetStaticMesh(m);
		BladeStart[leftHand] = m->FindSocket("BladeStart");
		BladeEnd[leftHand] = m->FindSocket("BladeEnd");
		check((BladeStart[leftHand] == nullptr) == (BladeEnd[leftHand] == nullptr));
	}
	inline void EquipItemDoubleHanded(TObjectPtr < UItemObject > item) {
		UnequipLeftHand();
		EquipItemRight(item);
	}
	inline void EquipItemDoubleHanded(UStaticMesh* m) {
		EquipItemRight(m);
	}
	inline void EquipItemRight(TObjectPtr < UItemObject > item) {
		EquipItem(false, item);
	}
	inline void EquipItemRight(UStaticMesh* m) {
		EquipItem(false, m);
	}
	inline void EquipItemLeft(TObjectPtr < UItemObject > item) {
		EquipItem(true, item);
	}
	inline void EquipItemLeft(UStaticMesh* m) {
		EquipItem(true, m);
	}
	void Unequip(bool leftHand) {
		ItemMesh(leftHand)->SetStaticMesh(nullptr);
		BladeStart[leftHand] = nullptr;
		BladeEnd[leftHand] = nullptr;
	}
	void UnequipDoubleHanded() {
		check(BladeStart[true]==nullptr);
		UnequipRightHand();
	}
	void UnequipLeftHand() {
		Unequip(true);
	}
	void UnequipRightHand() {
		Unequip(false);
	}
	void EnableWeaponTrace(bool enableLeftHandHitDetection, bool enableRightHandHitDetection, bool enableLeftFootHitDetection, bool enableRightFootHitDetection) {
		enableHandHitDetection[true] |= enableLeftHandHitDetection;
		enableHandHitDetection[false] |= enableRightHandHitDetection;
		enableFootHitDetection[true] |= enableLeftFootHitDetection;
		enableFootHitDetection[false] |= enableRightFootHitDetection;
		hitDetectionTimer = 0;
	}
	void DisableWeaponTrace() {
		enableHandHitDetection[0] = false;
		enableHandHitDetection[1] = false;
		enableFootHitDetection[0] = false;
		enableFootHitDetection[1] = false;
		hitDetectionTimer = INFINITY;
	}
	void NotifyAttackAnimationFinished(bool leftHanded) {
		DisableWeaponTrace();
	}
	const float INVINCIBILITY_AFTER_HIT = 0.2;
	float invincibilityDuration = 0;
	inline void HitDetectRightHand(TArray<FHitResult>& OutHit) {
		HitDetectHand(false, OutHit);
	}
	inline void HitDetectLeftHand(TArray<FHitResult>& OutHit) {
		HitDetectHand(true, OutHit);
	}
	inline bool HitDetectHand(bool leftHand, TArray<FHitResult>& OutHit) {
		if (IsEnabledHandHitDetection(leftHand)) {
			TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
			objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
			TArray<AActor*> actorsToIgnore;
			actorsToIgnore.Add(GetOwner());
			FVector s, e;
			GetStartEndLocation(leftHand, s, e);
			return UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), s, e, 50., objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true);
		}
		return false;
	}
	void HitDetectRightFoot() {

	}
	void HitDetectLeftFoot() {

	}
	
	
};
