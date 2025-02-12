// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/SkeletalMeshSocket.h"
#include "../anim/WeaponSetAnims.h"
#include "../items/ActorInventory.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../anim/AttackHandedness.h"
#include "SheathSocketType.h"
#include "Combat.generated.h"

#define HIT_DETECTION_PERIOD 0.1


USTRUCT(BlueprintType)
struct FAnyMesh {
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> ztatic;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> skeletal;
	
	bool isSkeletal;

	UPROPERTY()
	TObjectPtr<UItemObject> Item;
	
	EItemClass itemClass;

	inline UPrimitiveComponent * getComponent() {
		return isSkeletal ? (UPrimitiveComponent *) skeletal : ztatic;
	}
	inline bool create(TObjectPtr<UObject> outer) {
		if (isSkeletal) {
			if (skeletal == nullptr) {
				skeletal = NewObject<USkeletalMeshComponent>(outer, USkeletalMeshComponent::StaticClass());
				skeletal->SetSimulatePhysics(false);
				skeletal->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
				skeletal->RegisterComponent();
				return true;
			}
		}
		else {
			if (ztatic == nullptr) {
				ztatic = NewObject<UStaticMeshComponent>(outer, UStaticMeshComponent::StaticClass());
				ztatic->SetSimulatePhysics(false);
				ztatic->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
				ztatic->RegisterComponent();
				return true;
			}
		}
		return false;
	}
	inline bool equip(TObjectPtr<UObject> outer, TObjectPtr<UItemObject> item) {
		bool wasNewlyCreated = false;
		if (item == nullptr) {
			if (Item != nullptr) {
				if (isSkeletal) {
					skeletal->SetSkeletalMesh(nullptr);
				}
				else {
					ztatic->SetStaticMesh(nullptr);
				}
			}
		}else{
			FItem* row = item->Instance.getRow();
			isSkeletal = row->isSkeletal();
			itemClass = row->Class;
			wasNewlyCreated = create(outer);
			if (isSkeletal) {
				skeletal->SetSkeletalMesh(row->getSkeletalMesh());
			}
			else {
				ztatic->SetStaticMesh(row->getMesh());
			}
		}
		Item = item;
		return wasNewlyCreated;
	}
	
	inline UPrimitiveComponent* findSocket(FVector & location, FRotator & rotation, const FName itemSocket) {
		if (isSkeletal) {
			USkeletalMesh* const itemMesh = skeletal->SkeletalMesh;
			if (itemMesh) {
				const USkeletalMeshSocket* const socket = itemMesh->FindSocket(itemSocket);
				if (socket) {
					location = socket->RelativeLocation;
					rotation = socket->RelativeRotation;
				}
			}
			return skeletal;
		}
		else {
			UStaticMesh* const itemMesh = ztatic->GetStaticMesh();
			if (itemMesh) {
				const UStaticMeshSocket* const socket = itemMesh->FindSocket(itemSocket);
				if (socket) {
					location = socket->RelativeLocation;
					rotation = socket->RelativeRotation;
				}
			}
			return ztatic;
		}
	}
	inline void setTransform(FVector location, FRotator rotation) {
		UPrimitiveComponent* c = getComponent();
		c->SetRelativeLocation(location);
		c->SetRelativeRotation(rotation);
	}
	inline void attach(USkeletalMeshComponent* const characterMesh, const FName& itemSocket, const FName characterSocket) {
		if (Item) {
			FVector location;
			FRotator rotation;
			UPrimitiveComponent* comp = findSocket(location, rotation, itemSocket);
			const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
			comp->SetRelativeLocation(location);
			comp->SetRelativeRotation(rotation);
			comp->AttachToComponent(characterMesh, atr, characterSocket);
		}
	}
};

USTRUCT(BlueprintType)
struct FAnySocketh {
	GENERATED_BODY()

	UPROPERTY()
	const UStaticMeshSocket* ztatic;

	UPROPERTY()
	const USkeletalMeshSocket* skeletal;

	inline bool isNull(const FAnyMesh& meshComp) const {
		return meshComp.isSkeletal ? skeletal == nullptr : ztatic == nullptr;
	}
	inline FTransform GetSocketTransform(const FAnyMesh& meshComp) const {
		if (meshComp.isSkeletal) {
			return skeletal->GetSocketTransform(meshComp.skeletal);
		}
		else {
			FTransform trans;
			ztatic->GetSocketTransform(trans, meshComp.ztatic);
			return trans;
		}
	}
	inline void set(FName socketName, FAnyMesh & source) {
		if (source.isSkeletal) {
			USkeletalMesh* const itemMesh = source.skeletal->SkeletalMesh;
			if (itemMesh) {
				skeletal = itemMesh->FindSocket(socketName);
			}
		}
		else {
			UStaticMesh* const itemMesh = source.ztatic->GetStaticMesh();
			if (itemMesh) {
				const UStaticMeshSocket* const socket = itemMesh->FindSocket(socketName);
			}
		}
	}
	inline void unset(const FAnyMesh& source) {
		if (source.isSkeletal)skeletal = nullptr;
		else ztatic = nullptr;
	}

};
USTRUCT(BlueprintType)
struct GAME_API FCombatOneSide
{
	GENERATED_BODY()

	/** Hand socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FName HandSocket;
	/** Sheathed weapon socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocket;
	/** Sheathed weapon socket (back right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FName SheathedSocketBack;


	inline FName getSheathedSocketName() const {
		switch (HandMesh.itemClass) {
		default:
			return SheathedSocket;
		case EItemClass::BOW:
			return TEXT("bowSocket");
		case EItemClass::DOUBLE_HANDED_WEAPON:
		case EItemClass::SHIELD:
			return SheathedSocketBack;
		}
	}

	UPROPERTY()
	FAnyMesh HandMesh;

	FAnySocketh BladeStart;
	FAnySocketh BladeEnd;
	const USkeletalMeshSocket* BareHandSocket;

	bool enableHandHitDetection;
	bool enableFootHitDetection;

	bool isSheathed = true;
	
	inline FAnySocketh& getBladeSocket(bool start) {
		return start ? BladeStart : BladeEnd;
	}
	inline const FAnySocketh& getBladeSocket(bool start) const{
		return start ? BladeStart : BladeEnd;
	}

	inline FVector getHandLocation(const USkeletalMeshComponent* const playerMesh) const {
		return BareHandSocket->GetSocketLocation(playerMesh);
	}
	inline FTransform getBladeTransform(bool start) const {
		return getBladeSocket(start).GetSocketTransform(HandMesh);
	}
	inline FVector getBladeLocation(bool start) const {
		return getBladeTransform(start).GetLocation();
	}
	inline void GetStartEndLocation(FVector& start, FVector& end, const USkeletalMeshComponent* const playerMesh) const {
		if (BladeStart.isNull(HandMesh)) {
			end = start = getHandLocation(playerMesh);	
		}
		else {
			start = getBladeLocation(true);
			end = getBladeLocation(false);
		}
	}

	inline void unsheath(USkeletalMeshComponent* const characterMesh) {
		HandMesh.attach(characterMesh, "Handle", HandSocket);
	}
	inline void sheath(USkeletalMeshComponent* const characterMesh) {
		HandMesh.attach(characterMesh, "Handle", getSheathedSocketName());
	}
	inline void EquipItem(TObjectPtr<UObject> outer, USkeletalMeshComponent* const characterMesh, TObjectPtr<UItemObject> item) {
		if (item == nullptr) {
			Unequip();
		}
		else {
			if (HandMesh.equip(outer, item)) {
				HandMesh.attach(characterMesh, "Handle", isSheathed ? getSheathedSocketName() : HandSocket);
			}
			BladeStart.set("BladeStart", HandMesh);
			BladeEnd.set("BladeEnd", HandMesh);
			check(BladeStart.isNull(HandMesh) == BladeEnd.isNull(HandMesh));
		}
	}
	void Unequip() {
		BladeStart.unset(HandMesh);
		BladeEnd.unset(HandMesh);
		HandMesh.equip(nullptr, nullptr);
	}
	inline bool isEquipped() {
		return HandMesh.Item != nullptr;
	}
	void EnableWeaponTrace(bool EnableHandHitDetection, bool EnableFootHitDetection) {
		enableHandHitDetection |= EnableHandHitDetection;
		enableFootHitDetection |= EnableFootHitDetection;
	}
	inline bool IsSheathed() const {
		return isSheathed;
	}
	void DisableWeaponTrace() {
		enableHandHitDetection = false;
		enableFootHitDetection = false;
	}
	

	inline bool HitDetectHand(AActor* owner, UWorld* world, USkeletalMeshComponent* const mesh, TArray<FHitResult>& OutHit) {
		if (enableHandHitDetection) {
			TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
			objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
			TArray<AActor*> actorsToIgnore;
			actorsToIgnore.Add(owner);
			FVector s, e;
			GetStartEndLocation(s, e, mesh);
			return UKismetSystemLibrary::SphereTraceMultiForObjects(world, s, e, 50., objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true);
		}
		return false;
	}

	
};

USTRUCT(BlueprintType)
struct GAME_API FCombat
{
	GENERATED_BODY()


	unsigned int comboAnimIdx = 0;
	FCombat();
	class AGameCharacter* GameCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> BowDrawArrow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> BowShootArrow;

	/** Hand socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FCombatOneSide Left;

	/** Hand socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = "true"))
	FCombatOneSide Right;

	/** Death Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> DeathAnims;

	/** Hit Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> HitAnims;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
	FWeaponSetAnims WeaponAnims;

	float hitDetectionTimer;
	float invincibilityDuration = 0;

	inline FCombatOneSide& getDominantSide(TObjectPtr<UItemObject> item) {
		return GetSide(item->isLeftTheDominantHand());
	}
	inline FCombatOneSide& GetSide(bool leftHand) {
		return leftHand ? Left: Right;
	}
	inline const FName& SheathedSocketName(bool leftHand) {
		return GetSide(leftHand).getSheathedSocketName();
	}
	inline FAnyMesh & ItemMesh(bool leftHand)  {
		return GetSide(leftHand).HandMesh;
	}

	inline TObjectPtr<UAnimMontage> getHitAnim() {
		return HitAnims.Num() == 0 ? nullptr : HitAnims[comboAnimIdx++ % HitAnims.Num()];
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
	inline FWeaponAnim* GetNextAnim(FWeaponAnims* anims) {
		return anims->GetAnim(comboAnimIdx++);
	}
	inline FWeaponAnims* getAttackAnims(EMeleeAttackClass weaponClass) {
		return WeaponAnims.getAttackAnims(weaponClass);
	}
	inline FWeaponAnim* getAttackOrUnsheathAnim(EMeleeAttackClass weaponClass) {
		return isSheathed() ? getUnsheathAnim(weaponClass) : getAttackAnim(weaponClass);
	}
	inline bool isSheathed() const {
		return Left.IsSheathed() || Right.IsSheathed();
	}
	void EnableWeaponTrace(bool enableLeftHandHitDetection, bool enableRightHandHitDetection, bool enableLeftFootHitDetection, bool enableRightFootHitDetection) {
		Left.EnableWeaponTrace(enableLeftHandHitDetection, enableLeftFootHitDetection);
		Right.EnableWeaponTrace(enableRightHandHitDetection, enableRightFootHitDetection);
		hitDetectionTimer = 0;
	}
	void DisableWeaponTrace() {
		Left.DisableWeaponTrace();
		Right.DisableWeaponTrace();
		hitDetectionTimer = INFINITY;
	}

	void NotifyAttackAnimationFinished() {
		DisableWeaponTrace();
	}

};
