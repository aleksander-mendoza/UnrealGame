// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/SkeletalMeshSocket.h"
#include "../anim/WeaponSetAnims.h"
#include "../items/ActorInventory.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../anim/AttackHandedness.h"
#include "Combat.generated.h"

#define HIT_DETECTION_PERIOD 0.1

USTRUCT(BlueprintType)
struct GAME_API FCombat
{
	GENERATED_BODY()

	



	unsigned int comboAnimIdx = 0;
	FCombat();
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FName HandSocketR;
	/** Hand socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FName HandSocketL ;
	/** Sheathed weapon socket (left)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocketL;
	/** Sheathed weapon socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocketR ;
	/** Sheathed weapon socket (back left)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocketBackL;

	/** Sheathed weapon socket (back right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocketBackR;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> LeftHandMesh;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> RightHandMesh;
	inline FName & SheathedSocketName(bool leftHand, bool back) {
		return leftHand ? (back? SheathedSocketBackL : SheathedSocketL) : (back ? SheathedSocketBackR : SheathedSocketR);
	}
	inline TObjectPtr<UStaticMeshComponent>  ItemMesh(bool leftHand) const {
		return leftHand ? LeftHandMesh : RightHandMesh;
	}

	const UStaticMeshSocket* BladeStart[2];
	const UStaticMeshSocket* BladeEnd[2];
	const USkeletalMeshSocket* BareHandSocket[2];

	bool enableHandHitDetection[2];
	bool enableFootHitDetection[2];
	inline bool HasWeaponEquipped(bool leftHand) const {
		return BladeStart[leftHand]!=nullptr;
	}
	inline const FName & GetHandSocketName(bool leftHand) const {
		return leftHand ? HandSocketL : HandSocketR;
	}
	inline const USkeletalMeshSocket* GetHandSocket(bool leftHand) const{
		return BareHandSocket[leftHand];
	}
	inline const UStaticMeshSocket* GetBladeStart(bool leftHand) const {
		return BladeStart[leftHand];
	}
	inline const UStaticMeshSocket* GetBladeEnd(bool leftHand) const {
		return BladeEnd[leftHand];
	}
	inline FVector GetHandLocation(bool leftHand, const USkeletalMeshComponent*const mesh) const{
		return GetHandSocket(leftHand)->GetSocketLocation(mesh);
	}
	inline FVector GetBladeStartLocation(bool leftHand) const {
		FTransform trans;
		GetBladeStart(leftHand)->GetSocketTransform(trans, ItemMesh(leftHand));
		return trans.GetLocation();
	}
	inline FVector GetBladeEndLocation(bool leftHand) const {
		FTransform trans;
		GetBladeEnd(leftHand)->GetSocketTransform(trans, ItemMesh(leftHand));
		return trans.GetLocation();
	}
	inline void GetStartEndLocation(bool leftHand, FVector & start, FVector& end, const USkeletalMeshComponent*const mesh) const{
		if (HasWeaponEquipped(leftHand)) {
			start = GetBladeStartLocation(leftHand);
			end = GetBladeEndLocation(leftHand);
		}
		else {
			end = start = GetHandLocation(leftHand, mesh);
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
	

	bool IsSheathed[2] = { true, true };
	inline bool isSheathed() const {
		return IsSheathed[0]|| IsSheathed[1];
	}
	inline bool isSheathed(bool leftHand) const{
		return IsSheathed[leftHand];
	}
	inline void unsheath(bool leftHand, USkeletalMeshComponent* const mesh) {
		const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
		ItemMesh(leftHand)->AttachToComponent(mesh, atr, GetHandSocketName(leftHand));
		IsSheathed[leftHand] = false;
	}
	inline void sheath(bool leftHand, bool back, USkeletalMeshComponent* const mesh) {
		const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
		ItemMesh(leftHand)->AttachToComponent(mesh, atr, SheathedSocketName(leftHand, back));
		IsSheathed[leftHand] = true;
	}
	inline void sheathLeft(USkeletalMeshComponent* const mesh, bool back=false) {
		sheath(true, back, mesh);
	}
	inline void sheathRight(USkeletalMeshComponent* const mesh, bool back = false) {
		sheath(false, back, mesh);
	}
	inline void sheathBackRight(USkeletalMeshComponent* const mesh) {
		sheath(false, true, mesh);
	}
	inline void sheathBackLeft(USkeletalMeshComponent* const mesh) {
		sheath(true, true, mesh);
	}
	inline void unsheathLeft(USkeletalMeshComponent* const mesh) {
		unsheath(true, mesh);
	}
	inline void unsheathRight(USkeletalMeshComponent* const mesh) {
		unsheath(false, mesh);
	}
	inline TObjectPtr<UAnimMontage> getHitAnim() {
		return HitAnims.Num()==0?nullptr: HitAnims[comboAnimIdx++ % HitAnims.Num()];
	}
	inline TObjectPtr<UAnimMontage> getDeathAnim() {
		return DeathAnims.Num() == 0 ? nullptr : DeathAnims[comboAnimIdx++ % DeathAnims.Num()];
	}
	inline FWeaponAnim* getSheathAnim(EMeleeAttackClass weaponClass) {
		return WeaponAnims.getSheathAnim(weaponClass);
	}
	inline FWeaponAnim* getUnsheathAnim(EMeleeAttackClass weaponClass) {
		return WeaponAnims.getUnsheathAnim(weaponClass);
	}
	inline FWeaponAnim* getAttackAnim(EMeleeAttackClass weaponClass) {
		return WeaponAnims.getAttackAnim(weaponClass, comboAnimIdx++);
	}
	inline FWeaponAnim* GetNextAnim(FWeaponAnims * anims) {
		return anims->GetAnim(comboAnimIdx++);
	}
	inline FWeaponAnims* getAttackAnims(EMeleeAttackClass weaponClass) {
		return WeaponAnims.getAttackAnims(weaponClass);
	}
	inline FWeaponAnim * getAttackOrUnsheathAnim(EMeleeAttackClass weaponClass) {
		return isSheathed() ? getUnsheathAnim(weaponClass) : getAttackAnim(weaponClass);
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
	void NotifyAttackAnimationFinished() {
		DisableWeaponTrace();
	}
	
	float invincibilityDuration = 0;
	inline void HitDetectRightHand(AActor* owner, UWorld* world, USkeletalMeshComponent* const mesh, TArray<FHitResult>& OutHit) {
		HitDetectHand(false, owner, world, mesh, OutHit);
	}
	inline void HitDetectLeftHand(AActor* owner, UWorld* world, USkeletalMeshComponent* const mesh, TArray<FHitResult>& OutHit) {
		HitDetectHand(true, owner, world, mesh, OutHit);
	}
	inline bool HitDetectHand(bool leftHand, AActor*owner, UWorld * world, USkeletalMeshComponent* const mesh, TArray<FHitResult>& OutHit) {
		if (IsEnabledHandHitDetection(leftHand)) {
			TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
			objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
			TArray<AActor*> actorsToIgnore;
			actorsToIgnore.Add(owner);
			FVector s, e;
			GetStartEndLocation(leftHand, s, e, mesh);
			return UKismetSystemLibrary::SphereTraceMultiForObjects(world, s, e, 50., objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true);
		}
		return false;
	}
	void HitDetectRightFoot() {

	}
	void HitDetectLeftFoot() {

	}
	
	
};
