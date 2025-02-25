// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshedInventory.h"
#include "../item/Item.h"
#include "../item/ClothingItem.h"
#include "../item/WeaponItem.h"
#include "../ItemInstance.h"



void UMeshedInventory::unsheathLeft()
{
	const UWeaponAnims* mov = getCurrentMoveset();
	Left.HandMesh.attach(PlayerMesh, mov->UnsheathedHandleSocket, mov->LeftUnsheathSocket);
	Left.isSheathed = false;
}

void UMeshedInventory::unsheathRight()
{
	const UWeaponAnims* mov = getCurrentMoveset();
	Right.HandMesh.attach(PlayerMesh, mov->UnsheathedHandleSocket, mov->RightUnsheathSocket);
	Right.isSheathed = false;
}

void UMeshedInventory::sheathLeft()
{
	const UWeaponAnims* mov = getCurrentMoveset();
	Left.HandMesh.attach(PlayerMesh, mov->SheathedHandleSocket, mov->LeftSheathSocket);
	Left.isSheathed = true;
}

void UMeshedInventory::sheathRight()
{
	const UWeaponAnims* mov = getCurrentMoveset();
	Right.HandMesh.attach(PlayerMesh, mov->SheathedHandleSocket, mov->RightSheathSocket);
	Right.isSheathed = true;
}


void UMeshedInventory::InitializeComponent()
{
	Super::InitializeComponent();
	check(BareHandedMoveset != nullptr);
	Left.BareHandSocket = getPlayerMesh()->GetSocketByName(BareHandedMoveset->LeftUnsheathSocket);
	Right.BareHandSocket = getPlayerMesh()->GetSocketByName(BareHandedMoveset->RightUnsheathSocket);
	//check(Left.BareHandSocket != nullptr);
	//check(Right.BareHandSocket != nullptr);
}


bool UMeshedInventory::onHit(UInventoryAndHealth* attacker, UItemInstance* weaponUsed, UItemInstance* projectile, float damage)
{
	if (invincibilityDuration <= 0) {
		invincibilityDuration = INVINCIBILITY_AFTER_HIT;
		return true;
	}
	return false;
}

void UMeshedInventory::removeClothingItem(const UClothingItem* type, TObjectPtr<UItemInstance> item)
{
	check(item->ItemType == type);
	check(PlayerMesh != nullptr);
	const int idx = item->EquippedAt;
	check(idx >= 0);
	check(Clothes[idx] == item);
	ClothesMeshes[idx]->UnregisterComponent();
	ClothesMeshes.RemoveAtSwap(idx);
	Super::removeClothingItem(type, item);
}

void UMeshedInventory::addClothingItem(const UClothingItem* type, TObjectPtr<UItemInstance> item)
{
	check(item->ItemType == type);
	USkeletalMesh* clothingMesh = type->getSkeletalMesh();
	check(clothingMesh != nullptr);
	check(PlayerMesh != nullptr);
	USkeletalMeshComponent* clothingComp = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass());
	clothingComp->RegisterComponent();
	clothingComp->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
	clothingComp->SetSkeletalMesh(clothingMesh);
	clothingComp->SetAnimInstanceClass(PlayerMesh->AnimClass);
	clothingComp->SetLeaderPoseComponent(PlayerMesh, true);
	Super::addClothingItem(type, item);
	const int idx = ClothesMeshes.Add(clothingComp);
	checkf(item->EquippedAt == idx, TEXT("%d != %d"), item->EquippedAt, idx);
	check(Clothes[item->EquippedAt] == item);

}

bool UMeshedInventory::onUnequipProjectile()
{
	return Super::onUnequipProjectile();
}

bool UMeshedInventory::onUnequipDoubleHanded()
{
	if (Super::onUnequipDoubleHanded()) {
		updateCurrentMoveset();
		Left.unset();
		Right.unset();
		DisableWeaponTrace();
		return true;
	}
	return false;
}

bool UMeshedInventory::onUnequipLeftHand()
{
	if (Super::onUnequipLeftHand()) {
		updateCurrentMoveset();
		Left.unset();
		Left.DisableWeaponTrace();
		return true;
	}
	return false;
}

bool UMeshedInventory::onUnequipRightHand()
{
	if (Super::onUnequipRightHand()) {
		updateCurrentMoveset();
		Right.unset();
		Right.DisableWeaponTrace();
		return true;
	}
	return false;
}

bool UMeshedInventory::onEquipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance> owner)
{
	return Super::onEquipProjectile(type, owner);
}

bool UMeshedInventory::onEquipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::onEquipDoubleHanded(type, owner)) {
		updateCurrentMoveset();
		if (type->isLeftTheDominantHand()) {
			if (Left.setItem(this, getPlayerMesh(), Cast<UItem>(type))) {
				resheathLeft();
			}
		}
		else {
			if (Right.setItem(this, getPlayerMesh(), Cast<UItem>(type))) {
				resheathRight();
			}
		}
		return true;
	}
	return false;
}


bool UMeshedInventory::onEquipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::onEquipLeftHand(type, owner)) {
		updateCurrentMoveset();
		if (Left.setItem(this, getPlayerMesh(), Cast<UItem>(type))) {
			resheathLeft();
		}
		return true;
	}
	return false;
}

bool UMeshedInventory::onEquipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::onEquipRightHand(type, owner)) {
		updateCurrentMoveset();
		if (Right.setItem(this, getPlayerMesh(), Cast<UItem>(type))) {
			resheathRight();
		}
		return true;
	}
	return false;
}

void UMeshedInventory::stripHands()
{
	Super::stripHands();
	CurrentMoveset = BareHandedMoveset;
	DisableWeaponTrace();
	Left.unset();
	Right.unset();
}

void UMeshedInventory::stripClothes()
{
	Super::stripClothes();
	for (int i = 0; i < ClothesMeshes.Num(); i++) {
		ClothesMeshes[i]->UnregisterComponent();
		ClothesMeshes.RemoveAtSwap(i);
	}
}


