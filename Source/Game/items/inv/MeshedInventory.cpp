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
	check(Left.BareHandSocket != nullptr);
	check(Right.BareHandSocket != nullptr);
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

bool UMeshedInventory::unequipProjectile()
{
	return Super::unequipProjectile();
}

bool UMeshedInventory::unequipDoubleHanded()
{
	if (Super::unequipDoubleHanded()) {
		updateCurrentMoveset();
		return true;
	}
	return false;
}

bool UMeshedInventory::unequipLeftHand()
{
	if (Super::unequipLeftHand()) {
		updateCurrentMoveset();
		return true;
	}
	return false;
}

bool UMeshedInventory::unequipRightHand()
{
	if (Super::unequipRightHand()) {
		updateCurrentMoveset();
		return true;
	}
	return false;
}

bool UMeshedInventory::equipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance> owner)
{
	return Super::equipProjectile(type, owner);
}

bool UMeshedInventory::equipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::equipDoubleHanded(type, owner)) {
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


bool UMeshedInventory::equipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::equipLeftHand(type, owner)) {
		updateCurrentMoveset();
		if (Left.setItem(this, getPlayerMesh(), Cast<UItem>(type))) {
			resheathLeft();
		}
		return true;
	}
	return false;
}

bool UMeshedInventory::equipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Super::equipRightHand(type, owner)) {
		updateCurrentMoveset();
		if (Right.setItem(this, getPlayerMesh(), Cast<UItem>(type))) {
			resheathRight();
		}
		return true;
	}
	return false;
}

