// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterInventory.h"
#include "hand/HandInventory.h"
#include "../../anim/moveset/WeaponAnim.h"
#include "MeshedInventory.generated.h"

#define HIT_DETECTION_PERIOD 0.1
#define INVINCIBILITY_AFTER_HIT 0.2

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UMeshedInventory : public UCharacterInventory
{
	GENERATED_BODY()

	////////////////////////////////////////
	/////////////// PROPERTIES /////////////
	////////////////////////////////////////

	UPROPERTY()
	USkeletalMeshComponent* PlayerMesh;

public:	


	UPROPERTY()
	TArray<TObjectPtr<USkeletalMeshComponent>> ClothesMeshes;

	/** Hand socket (right)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FHandInventory Left;

	/** Hand socket (right)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FHandInventory Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWeaponAnims* BareHandedMoveset;

	UPROPERTY()
	const UWeaponAnims* CurrentMoveset;

	////////////////////////////////////////
	/////////////// GETTERS  ///////////////
	////////////////////////////////////////


	inline EArmedPoseType getArmedPoseType() const{
		return CurrentMoveset->ArmedPose;
	}
	inline USkeletalMeshComponent* getPlayerMesh() {
		return PlayerMesh;
	}
	void setPlayerMesh(USkeletalMeshComponent* playerMesh) {
		PlayerMesh = playerMesh;
		CurrentMoveset = BareHandedMoveset;
	}
	inline UAnimInstance * getAnimInstance() {
		return PlayerMesh->GetAnimInstance();
	}
	inline void updateCurrentMoveset() {
		if (LeftHand == nullptr) {
			if (RightHand == nullptr) {
				CurrentMoveset = BareHandedMoveset;
			}
			else {
				CurrentMoveset = RightHand->getMoveset();
			}
		}
		else {
			if (RightHand == nullptr) {
				CurrentMoveset = LeftHand->getMoveset();
			}
			else {
				CurrentMoveset = RightHand->getMoveset()->max(LeftHand->getMoveset());
			}
		}
	}
	inline const UWeaponAnims* getCurrentMoveset() const {
		check(CurrentMoveset != nullptr);
		return CurrentMoveset;
	}
	inline bool IsSheathedLeft() const {
		return Left.IsSheathed();
	}
	inline bool IsSheathedRight() const {
		return Right.IsSheathed();
	}
	inline bool IsSheathed() const {
		return IsSheathedLeft() || IsSheathedRight();
	}

	////////////////////////////////////////
	/////////////// SHEATHING  /////////////
	////////////////////////////////////////


	virtual void unsheathLeft();
	virtual void unsheathRight();
	virtual void sheathLeft();
	virtual void sheathRight();

	inline void unsheathBoth() {
		unsheathLeft();
		unsheathRight();
	}
	inline void sheathBoth() {
		sheathLeft();
		sheathRight();
	}
	

	inline void resheathLeft() {
		if (IsSheathedLeft())sheathLeft();
		else unsheathLeft();
	}
	inline void resheathRight() {
		if (IsSheathedRight())sheathRight();
		else unsheathRight();
	}
	inline void sheath(bool unsheath, bool left, bool right) {
		if (unsheath) {
			if (left)unsheathLeft();
			if (right)unsheathRight();
		}
		else {
			if (left)sheathLeft();
			if (right)sheathRight();
		}
	}

	////////////////////////////////////////
	/////////////// WEAPON TRACE ///////////
	////////////////////////////////////////


	float hitDetectionTimer = INFINITY;
	float invincibilityDuration = 0;

	inline void EnableWeaponTrace(bool enableLeftHandHitDetection, bool enableRightHandHitDetection, bool enableLeftFootHitDetection, bool enableRightFootHitDetection) {
		Left.EnableWeaponTrace(enableLeftHandHitDetection, enableLeftFootHitDetection);
		Right.EnableWeaponTrace(enableRightHandHitDetection, enableRightFootHitDetection);
		hitDetectionTimer = 0;
	}
	inline void DisableWeaponTrace() {
		Left.DisableWeaponTrace();
		Right.DisableWeaponTrace();
		hitDetectionTimer = INFINITY;
	}

	


	////////////////////////////////////////
	/////////////// EVENTS     /////////////
	////////////////////////////////////////


	virtual void InitializeComponent() override;

	virtual bool onHit(UInventoryAndHealth * attacker, class UItemInstance* weaponUsed, class UItemInstance* projectile, float damage);

	virtual void removeClothingItem(const class UClothingItem* type, TObjectPtr < UItemInstance > item) override;

	virtual void addClothingItem(const class UClothingItem* type, TObjectPtr < UItemInstance> item) override;

	virtual bool onUnequipProjectile() override;

	virtual bool onUnequipDoubleHanded() override;

	virtual bool onUnequipLeftHand() override;

	virtual bool onUnequipRightHand() override;

	virtual bool onEquipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual bool onEquipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual bool onEquipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual bool onEquipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual void stripHands() override;

	virtual void stripClothes() override;

	virtual void NotifyAttackAnimationFinished() {
		DisableWeaponTrace();
	}
	inline void TickInvincibility(float DeltaTime) {
		if(invincibilityDuration>0)invincibilityDuration -= DeltaTime;
	}


};
