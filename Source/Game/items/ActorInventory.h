// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemObject.h"
#include "ItemInstanceList.h"
#include "Container.h"
#include "../anim/ArmedPoseType.h"
#include "ActorInventory.generated.h"

//DECLARE_DELEGATE_OneParam(FStringDelegate, FString);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UActorInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	class UInventory * currentWidget=nullptr;
	// Sets default values for this component's properties
	UActorInventory();

	UPROPERTY()
	TArray< TObjectPtr<UItemObject>> Items;


	/**These are items that are currently worn as clothes*/
	UPROPERTY()
	TArray< TObjectPtr<UItemObject>> Clothes;

	/**These is the item that is currently held in left hand*/
	UPROPERTY()
	TObjectPtr < UItemObject> LeftHand;

	/**These is the item that is currently held in right hand (it could be the same object as in left hand. This is the case for double-handed weapons)*/
	UPROPERTY()
	TObjectPtr < UItemObject> RightHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FItemInstanceList DefaultItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FItemInstanceList DefaultClothes;

	int32 occupiedClothingSlots = 0;

	virtual void PostInitProperties() override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	ContainerEvents* containerEvents;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	inline TObjectPtr < UItemObject>  getHandItem(bool leftHand) {
		return leftHand ? LeftHand : RightHand;
	}
	inline EItemClass getWeaponClass(bool leftHand) {
		return leftHand ? getLeftHandWeaponClass() : getRightHandWeaponClass();
	}

	inline EArmedPoseType getArmedPoseType() {
		const EItemClass left = getLeftHandWeaponClass();
		const EItemClass right = getRightHandWeaponClass();
		if (left == EItemClass::BOW) {
			return EArmedPoseType::BOW;
		}else if (left == EItemClass::DOUBLE_HANDED_WEAPON) {
			return EArmedPoseType::DOUBLE_HANDED;
		}
		else if (left == EItemClass::SINGLE_HANDED_QUIET_WEAPON || left == EItemClass::SINGLE_HANDED_WEAPON ||
			right == EItemClass::SINGLE_HANDED_QUIET_WEAPON || right == EItemClass::SINGLE_HANDED_WEAPON) {
			return EArmedPoseType::SINGLE_HANDED;
		}
		else {
			return EArmedPoseType::BARE_HANDED;
		}
	}
	inline bool IsDoubleHanded() {
		return LeftHand != nullptr && LeftHand == RightHand;
	}
	inline EItemClass getLeftHandWeaponClass() {
		return LeftHand ==nullptr? EItemClass::NONE : LeftHand->getItemClass();
	}
	inline EItemClass getRightHandWeaponClass() {
		return RightHand == nullptr ? EItemClass::NONE : RightHand->getItemClass();
	}
	inline float getWeaponDamage(bool leftHand, float bareHandDamage) {
		return leftHand ? getLeftHandWeaponDamage(bareHandDamage) : getRightHandWeaponDamage(bareHandDamage);
	}
	inline float getLeftHandWeaponDamage(float bareHandDamage) {
		return LeftHand == nullptr ? bareHandDamage : LeftHand->getItemDamage();
	}
	inline float getRightHandWeaponDamage(float bareHandDamage) {
		return RightHand == nullptr ? bareHandDamage : RightHand->getItemDamage();
	}
	
	inline void ResetToDefaultItems(UWorld* world)
	{
		for (int i = 0; i < DefaultItems.Instances.Num(); i++) {
			spawnItem(world, DefaultItems.Instances[i]);
		}
	}

	inline void ResetToDefaultClothes(UWorld* world)
	{
		for (int i = 0; i < DefaultClothes.Instances.Num(); i++) {
			UItemObject* item = spawnItem(world, DefaultClothes.Instances[i]);
			const bool b = addClothingItem<true>(item);
			check(b);
		}
	}
	inline void ResetToDefault(UWorld* world) {
		ResetToDefaultItems(world);
		ResetToDefaultClothes(world);
	}
	inline FItem * getClothesAt(int clothesIdx) {
		return Clothes[clothesIdx]->getRow();
	}
	inline FItem* getItemAt(int itemIdx) {
		return Items[itemIdx]->getRow();
	}
	inline FItem* getLefHand() {
		return LeftHand->getRow();
	}
	inline FItem* getRightHand() {
		return RightHand->getRow();
	}
	/**returns true if the clothing item is not in conflicts with the currently worn items*/
	inline bool canPutOn(const FItem& item) const {
		return (occupiedClothingSlots & item.ClothingSlots) == 0;
	}

	/**returns true if there are no equipped devious clothes that conflict with this item*/
	inline bool canMakeSpaceFor(const FItem& item) {
		for (int i = 0; i < Clothes.Num(); i++) {
			if (getClothesAt(i)->hardConflictsWith(item))return false;
		}
		return true;
	}
	inline bool isAllValid() {
		for (int i = 0; i < Items.Num(); i++) {
			check(Items[i]->container == this);
		}
		int cs = 0;
		for (int i = 0; i < Clothes.Num(); i++) {
			cs |= Clothes[i]->getRow()->ClothingSlots;
			check(Items.Contains(Clothes[i]));
			check(Clothes[i]->equippedAt==i);
		}
		check(cs == occupiedClothingSlots);
		if (IsDoubleHanded()) {
			check(Items.Contains(LeftHand));
			check(LeftHand->equippedAt == -4);
		}
		else {
			if (LeftHand != nullptr) {
				check(Items.Contains(LeftHand));
				check(LeftHand->equippedAt == -2);
			}
			if (RightHand != nullptr) {
				check(Items.Contains(RightHand));
				check(RightHand->equippedAt == -1);
			}
		}
		return true;
	}
	
	template<bool onlyIfNotDevious>
	inline bool removeClothingItem(TObjectPtr < UItemObject > item) {
		check(item->container == this);
		if (0 <= item->equippedAt && item->equippedAt < Clothes.Num() && Clothes[item->equippedAt]==item) {
			const FItem& i = *item->Instance.getRow();
			if (onlyIfNotDevious && i.isDevious()) return false;
			if (containerEvents)containerEvents->OnUnequipClothes(item);
			check(Clothes[item->equippedAt] == item);
			const int idx = item->equippedAt;
			if(Clothes.Last()->equippedAt>=0)Clothes.Last()->equippedAt = idx;
			Clothes.RemoveAtSwap(idx);
			check(idx>= Clothes.Num() || Clothes[idx]->equippedAt<0 || Clothes[idx]->equippedAt== idx);
			item->equippedAt = -3;
			occupiedClothingSlots &= ~i.ClothingSlots;
			check(isAllValid());
			return true;
		}
		return false;
	}
	template<bool onlyIfNotDevious>
	inline bool makeSpaceFor(const FItem& item) {
		for (int clothesIdx = 0; clothesIdx < Clothes.Num(); clothesIdx++) {
			UItemObject* equippedItemObj = Clothes[clothesIdx];
			const FItem* equippedItem = equippedItemObj->Instance.getRow();
			if (equippedItem->conflictsWith(item)) {
				if (!removeClothingItem<onlyIfNotDevious>(equippedItemObj)) {
					return false;
				}
			}
		}
		return true;
	}
	template<bool onlyIfNotConflicting>
	inline bool addClothingItem(TObjectPtr < UItemObject> item) {
		check(item->container == this);
		if (item->equippedAt == -3) {
			const FItem & i = *item->Instance.getRow();
			if (onlyIfNotConflicting && !canPutOn(i))return false;
			item->equippedAt = Clothes.Add(item);
			occupiedClothingSlots |= i.ClothingSlots;
			check(Items.Contains(item));
			if (containerEvents)containerEvents->OnEquipClothes(item);
			check(isAllValid());
			return true;
		}
		return false;
	}
	inline void addItem(TObjectPtr<UItemObject> item) {
		check(item->container == nullptr);
		check(item->equippedAt == -3);
		Items.Add(item);
		item->container = this;
		if (containerEvents)containerEvents->OnPutItem(item);
	}
	inline void emptyBothHands() {
		check(LeftHand == RightHand);
		if (LeftHand != nullptr) {
			TObjectPtr<UItemObject> i = LeftHand;
			RightHand = nullptr;
			LeftHand = nullptr;
			i->equippedAt = -3;
			if (containerEvents)containerEvents->OnUnequipBothHands(i);
			check(isAllValid());
		}
	}
	inline void emptyLeftHand() {
		if (LeftHand != nullptr) {
			if (LeftHand == RightHand) {
				emptyBothHands();
			}
			else {
				LeftHand->equippedAt = -3;
				if (containerEvents)containerEvents->OnUnequipLeftHand(LeftHand);
				LeftHand = nullptr;
				check(isAllValid());
			}
		}
	}
	inline void emptyRightHand() {
		if (RightHand != nullptr) {
			if (LeftHand == RightHand) {
				emptyBothHands();
			}
			else {
				RightHand->equippedAt = -3;
				if (containerEvents)containerEvents->OnUnequipRightHand(RightHand);
				RightHand = nullptr;
				check(isAllValid());
			}
		}
	}
	inline void takeInHand(TObjectPtr<UItemObject> item, bool left) {
		check(item->container == this);
		FItem * i = item->getRow();
		if (i->isDoubleHanded()) {
			emptyRightHand();
			emptyLeftHand();
			LeftHand = item;
			RightHand = item;
			item->equippedAt = -4;
			if (containerEvents)containerEvents->OnEquipBothHands(item);
			check(isAllValid());
		}
		else if(left){
			emptyLeftHand();
			LeftHand = item;
			item->equippedAt = -2;
			if (containerEvents)containerEvents->OnEquipLeftHand(item);
		}
		else {
			emptyRightHand();
			RightHand = item;
			item->equippedAt = -1;
			if (containerEvents)containerEvents->OnEquipRightHand(item);
		}
	}
	template<bool onlyIfNotDevious>
	inline bool dropItem(TObjectPtr<UItemObject> item) {
		check(item->container == this);
		if (item->equippedAt == -4) {
			emptyBothHands();
		}else if (item->equippedAt == -2) {
			emptyLeftHand();
		}else if (item->equippedAt == -1) {
			emptyRightHand();
		}
		else if (item->equippedAt >= 0) {
			if (!removeClothingItem<onlyIfNotDevious>(item)) {
				return false;
			}
		}
		item->equippedAt = -3;
		item->container = nullptr;
		bool b = Items.RemoveSingleSwap(item)!=0;
		check(b);
		if (containerEvents)containerEvents->OnDropItem(item);
		check(isAllValid());
		return true;
	}

	inline UItemObject* spawnItem(UWorld* world, FItemInstance& instance) {
		UItemObject* item = NewObject<UItemObject>(world);
		item->Instance = instance;
		addItem(item);
		return item;
	}
	/**Tries to eqip item (if it's a clothing item it will be put on the character as clothes. If it is not a clothing item it will be held in hand). Returns false if the item couldn't be equipped. Potential reasons would be
	1. it was clothing item but not for this character (mismatched geneder/race)
	2. unequipClothesWithOverlappingSlots was set to false and the character was already wearing clothes with overlapping slots
	3. unequipClothesWithOverlappingSlots was set to true but one of the already equipped conflicting items was devious */
	bool equipItem(UItemObject* item, bool leftHand, bool unequipClothesWithOverlappingSlots = true);
	/**Returns false if the item couldn't be unequipped. This can happend in case of clothes that are marked as devious. Use force=true to force the lock open*/
	bool unequipItem(UItemObject* item, bool force = false);

	bool toggleItem(UItemObject* item, bool leftHand, bool unequipClothesWithOverlappingSlots = true, bool force = false) {
		if (item->equippedAt == -3) {
			return equipItem(item, leftHand, unequipClothesWithOverlappingSlots);
		}
		else {
			return unequipItem(item, force);
		}
	}

};
