// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueInventory.h"
#include "CharacterInventory.generated.h"

//DECLARE_DELEGATE_OneParam(FStringDelegate, FString);
class UDoubleHandedWeaponItem;
class UOneHandedWeaponItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UCharacterInventory  : public UDialogueInventory
{
	GENERATED_BODY()

public:	

	/**These are items that are currently worn as clothes*/
	UPROPERTY()
	TArray< TObjectPtr<UItemInstance>> Clothes;

	/**The item that will be shot from a bow*/
	UPROPERTY()
	TObjectPtr < UItemInstance> SelectedProjectile;

	/**These is the item that is currently held in left hand*/
	UPROPERTY()
	TObjectPtr < UItemInstance> LeftHand;
	
	/**These is the item that is currently held in right hand (it could be the same object as in left hand. This is the case for double-handed weapons)*/
	UPROPERTY()
	TObjectPtr < UItemInstance> RightHand;


	int32 occupiedClothingSlots = 0;
	int32 occupiedDeviousClothingSlots = 0;

	inline bool IsDoubleHanded() const {
		return LeftHand != nullptr && LeftHand == RightHand;
	}
	inline TObjectPtr<UItemInstance> getHandItem(bool leftHand) {
		return leftHand ? LeftHand : RightHand;
	}

	inline TObjectPtr<UItemInstance> decrementProjectileCount() {
		if (SelectedProjectile != nullptr) {
			TObjectPtr<UItemInstance> popped = SelectedProjectile->popCount(GetWorld());
			if (popped == SelectedProjectile) SelectedProjectile = nullptr;
			return popped;
		}
		return nullptr;
	}

	
	inline bool isAllValid() {
		check((occupiedClothingSlots | occupiedDeviousClothingSlots) == occupiedClothingSlots);
		int cs = 0;
		for (int i = 0; i < Clothes.Num(); i++) {
			cs |= Clothes[i]->ItemType->getClothingSlots();
			check(Items.Contains(Clothes[i]));
			check(Clothes[i]->EquippedAt==i);
		}
		check(cs == occupiedClothingSlots);
		if (IsDoubleHanded()) {
			check(Items.Contains(LeftHand));
			check(LeftHand->EquippedAt == EQUIPPED_AT_DOUBLEHANDED);
		}
		else {
			if (LeftHand != nullptr) {
				check(Items.Contains(LeftHand));
				check(LeftHand->EquippedAt == EQUIPPED_AT_LEFT_HAND);
			}
			if (RightHand != nullptr) {
				check(Items.Contains(RightHand));
				check(RightHand->EquippedAt == EQUIPPED_AT_RIGHT_HAND);
			}
		}
		return true;
	}

	virtual TObjectPtr<UItemInstance>  dropItem(TObjectPtr<UItemInstance> item, int quantity) override;

	virtual void removeClothingItem(const class UClothingItem* type, TObjectPtr < UItemInstance > item);
	
	virtual void addClothingItem(const class UClothingItem* type, TObjectPtr < UItemInstance> item);

	virtual bool equipClothes(const class UClothingItem* type, TObjectPtr < UItemInstance > owner);

	virtual bool unequipClothes(const class UClothingItem* type, TObjectPtr < UItemInstance> owner);

	virtual bool onUnequipProjectile();

	virtual bool onUnequipDoubleHanded();

	virtual bool onUnequipLeftHand();

	virtual bool onUnequipRightHand();

	virtual bool onEquipProjectile(const class UProjectileItem* type, TObjectPtr<UItemInstance>  owner);

	virtual bool onEquipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner);

	virtual bool onEquipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner);

	virtual bool onEquipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner);

	virtual void stripClothes();

	virtual void stripHands();

	virtual void stripAll() {
		stripHands();
		stripClothes();
		unequipProjectile();
	}

	virtual void clearInventory() override;

	inline void equipProjectile(const class UProjectileItem* type, TObjectPtr<UItemInstance>  owner) {
		unequipProjectile();
		onEquipProjectile(type, owner);
	}
	inline void unequipProjectile() {
		if (SelectedProjectile != nullptr) {
			onUnequipProjectile();
		}
	}

	inline bool unequipBothHands() {
		if (LeftHand != nullptr) {
			if (LeftHand == RightHand) {
				return onUnequipDoubleHanded();
			} else {
				if (!onUnequipLeftHand()) {
					return false;
				}
			}
		} 
		if (RightHand != nullptr) {
			return onUnequipRightHand();
		}
		return true;
	}
	inline bool unequipRightHand() {
		if (RightHand != nullptr) {
			if (RightHand == LeftHand) {
				return onUnequipDoubleHanded();
			}
			else {
				return onUnequipRightHand();
			}
		}
		return true;
	}
	inline bool unequipLeftHand() {
		if (LeftHand != nullptr) {
			if (LeftHand == RightHand) {
				return onUnequipDoubleHanded();
			}
			else {
				return onUnequipLeftHand();
			}
		}
		return true;
	}
	inline bool equipSingleHanded(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner, bool leftHand) {
		return leftHand ? equipLeftHand(type, owner) : equipRightHand(type, owner);
	}
	inline bool equipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) {
		return unequipLeftHand() && onEquipLeftHand(type, owner);
	}
	inline bool equipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) {
		return unequipRightHand() && onEquipRightHand(type, owner);
	}
	inline bool equipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) {
		return unequipBothHands() && onEquipDoubleHanded(type, owner);
	}

};
