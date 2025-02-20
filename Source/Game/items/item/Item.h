// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../anim/moveset/WeaponAnims.h"
#include "Item.generated.h"

class UGameCharacterInventory;
class UItemInstance;
class UInventoryAndHealth;
class UCharacterInventory;
/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Durbility = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Value = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Level = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight = 0;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> WearableMesh;

	
	virtual bool use(UCharacterInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const;

	
	virtual bool equip(UCharacterInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const { return false; }

	
	virtual bool unequip(UCharacterInventory* user, TObjectPtr<UItemInstance> owner) const { return false; }


	
	virtual TObjectPtr<UItemInstance> drop(UCharacterInventory* user, TObjectPtr<UItemInstance> owner, int count=1) const;

	
	virtual bool affect(UCharacterInventory* user, UCharacterInventory* affected, TObjectPtr<UItemInstance> owner) const {
		return use(affected, owner, true);
	}

	virtual bool startAttack(UGameCharacterInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const { return false; }
	virtual void endAttack(UGameCharacterInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const { }
	virtual void cancelAttack(UGameCharacterInventory* user, TObjectPtr<UItemInstance> owner) const { }
	
	
	UFUNCTION()
	virtual int getDeviousLevel() const {
		return 0;
	}
	UFUNCTION()
	virtual int32 getClothingSlots() const {
		return 0;
	}
	inline bool isDevious() const {
		return getDeviousLevel()!=0;
	}
	UFUNCTION()
	virtual const UWeaponAnims* getMoveset() const {
		return nullptr;
	}
	
	inline EArmedPoseType getArmedPoseType() const {
		const UWeaponAnims* moveset = getMoveset();
		return moveset == nullptr ? EArmedPoseType::UNARMED : moveset->ArmedPose;
	}
	UFUNCTION()
	virtual float getDamage() const {
		return 0;
	}
	UFUNCTION()
	virtual float getWarmth() const {
		return 0;
	}
	virtual const class UWeaponItem* asWeaponItem() const {
		return nullptr;
	}
	virtual const class UClothingItem * asClothingItem() const {
		return nullptr;
	}
	virtual const class UFoodItem* asFoodItem() const {
		return nullptr;
	}
	virtual bool isLeftTheDominantHand() const {
		return false;
	}
	virtual float getTotalDamage(UInventoryAndHealth* user) const {
		return 0;
	}
	inline USkeletalMesh* getSkeletalMesh() const {
		return WearableMesh.LoadSynchronous();
	}
	inline UStaticMesh* getMesh() const {
		return Mesh.LoadSynchronous();
	}
	inline bool isSkeletal() const{
		return Mesh.IsNull();
	}
	inline bool conflictsWith(const UItem& other) const {
		return (other.getClothingSlots() & getClothingSlots()) != 0;
	}
	inline bool hardConflictsWith(const UItem& other) const {
		return isDevious() && conflictsWith(other);
	}
};

