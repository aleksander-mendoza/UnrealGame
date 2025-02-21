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
	

	virtual void removeClothingItem(const class UClothingItem* type, TObjectPtr < UItemInstance > item);
	
	virtual void addClothingItem(const class UClothingItem* type, TObjectPtr < UItemInstance> item);

	virtual bool onEquipClothes(const class UClothingItem* type, TObjectPtr < UItemInstance > owner);

	virtual bool onUnequipClothes(const class UClothingItem* type, TObjectPtr < UItemInstance> owner);

	virtual bool onUnequipProjectile();

	virtual bool onUnequipDoubleHanded();

	virtual bool onUnequipSingleHanded(bool lefthand) {
		return lefthand ? onUnequipLeftHand() : onUnequipRightHand();
	}

	virtual bool onUnequipBothHands() {
		bool a = onUnequipLeftHand();
		bool b = onUnequipRightHand();
		return a && b;
	}

	virtual bool onUnequipLeftHand();

	virtual bool onUnequipRightHand();

	virtual bool onEquipProjectile(const class UProjectileItem* type, TObjectPtr<UItemInstance>  owner);

	virtual bool onEquipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner);

	virtual bool onEquipSingleHanded(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner, bool leftHand);

	virtual bool onEquipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner);

	virtual bool onEquipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner);

	virtual void unequipProjectile();

	virtual void unequipHands();

	virtual void unequipClothes();

	virtual void unequipAll() {
		unequipHands();
		unequipClothes();
		unequipProjectile();
	}

	virtual void clearInventory() override;
};
