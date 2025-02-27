// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "../../anim/moveset/WeaponAnims.h"
#include "WeaponItem.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UWeaponItem : public UItem
{
	GENERATED_BODY()
public:


	virtual float getDamage() const override{
		return Damage;
	}

	virtual const UWeaponAnims* getMoveset() const override {
		return Moveset;
	}
	/** Single-handed fighting animation*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UWeaponAnims * Moveset = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage = 0;

	virtual float getTotalDamage(UInventoryAndHealth* user) const override {
		return getDamage();
	}
	virtual bool startAttack(UCombatInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const override;
	virtual void endAttack(UCombatInventory* user, TObjectPtr<UItemInstance> owner, bool leftHand) const override;
	virtual bool isLeftTheDominantHand() const {
		return Moveset->IsLeftTheDominantHand;
	}
};

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UOneHandedWeaponItem : public UWeaponItem
{
	GENERATED_BODY()
public:

	

	virtual bool equip(UCharacterInventory* user, TObjectPtr<UItemInstance>  owner, bool leftHand) const override;

	virtual bool unequip(UCharacterInventory* user, TObjectPtr<UItemInstance>  owner) const override;

	virtual float getTotalDamage(UInventoryAndHealth* user) const override;
};

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UDoubleHandedWeaponItem : public UWeaponItem
{
	GENERATED_BODY()
public:


	virtual bool equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const override;

	virtual bool unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const override;

	virtual float getTotalDamage(UInventoryAndHealth* user) const override;
};

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UProjectileItem : public UWeaponItem
{
	GENERATED_BODY()
public:

	virtual bool equip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner, bool leftHand) const override;

	virtual bool unequip(UCharacterInventory* user,  TObjectPtr<UItemInstance>  owner) const override;


};
/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UBowItem : public UDoubleHandedWeaponItem
{
	GENERATED_BODY()
public:

	virtual float getTotalDamage(UInventoryAndHealth* user) const override;
};

/**
 *
 */
UCLASS(BlueprintType)
class GAME_API UStealthWeaponItem : public UOneHandedWeaponItem
{
	GENERATED_BODY()
public:
	virtual float getTotalDamage(UInventoryAndHealth* user) const override;
};


/**
 *
 */
UCLASS(BlueprintType)
class GAME_API USpearItem : public UDoubleHandedWeaponItem
{
	GENERATED_BODY()
public:

	virtual float getTotalDamage(UInventoryAndHealth* user) const override;
};