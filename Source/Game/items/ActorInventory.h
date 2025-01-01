// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemObject.h"
#include "ItemInstanceList.h"
#include "ActorInventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UActorInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorInventory();

	UPROPERTY()
	TArray<UItemObject*> Items;


	/**These are items that are currently worn as clothes*/
	UPROPERTY()
	TArray<UItemObject*> Clothes;

	/**These is the item that is currently held in left hand*/
	UPROPERTY()
	UItemObject* LeftHand;

	/**These is the item that is currently held in right hand (it could be the same object as in left hand. This is the case for double-handed weapons)*/
	UPROPERTY()
	UItemObject* RightHand;

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ResetToDefaultItems();
	void ResetToDefaultClothes();
	inline void ResetToDefault() {
		ResetToDefaultItems();
		ResetToDefaultClothes();
	}
	inline FItem * getClothesAt(int clothesIdx) {
		return Clothes[clothesIdx]->Instance.getRow();
	}
	inline FItem* getItemAt(int itemIdx) {
		return Items[itemIdx]->Instance.getRow();
	}
	inline FItem* getLefHand() {
		return LeftHand->Instance.getRow();
	}
	inline FItem* getRightHand() {
		return RightHand->Instance.getRow();
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
	template<bool onlyIfNotDevious>
	inline bool removeClothingItem(UItemObject * item) {
		if (item->equippedAt < Clothes.Num() && Clothes[item->equippedAt]==item) {
			const FItem& i = *item->Instance.getRow();
			if (onlyIfNotDevious && i.isDevious()) return false;
			Clothes.Last()->equippedAt = item->equippedAt;
			Clothes.RemoveAtSwap(item->equippedAt);
			item->equippedAt = -1;
			occupiedClothingSlots &= ~i.ClothingSlots;
			return true;
		}
		return false;
	}
	template<bool onlyIfNotConflicting>
	inline bool addClothingItem(UItemObject* item) {
		if (item->equippedAt < 0) {
			const FItem & i = *item->Instance.getRow();
			if (onlyIfNotConflicting && !canPutOn(i))return false;
			item->equippedAt = Clothes.Add(item);
			occupiedClothingSlots |= i.ClothingSlots;
			check(Items.Contains(item));
			return true;
		}
		return false;
	}
};
