// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryAndHealth.h"
#include "CharacterInventory.h"
#include "../../ui/NpcHealthBar.h"
#include "../item/ClothingItem.h"
#include "../item/WeaponItem.h"
#include "../../character/Hittable.h"
#include "../ItemInstance.h"



bool UInventoryAndHealth::onHit(UInventoryAndHealth* attacker, UItemInstance* weaponUsed, UItemInstance* projectile, float damage)
{
	if (Super::onHit(attacker, weaponUsed, projectile, damage)) {
		Health.Health -= damage;
		if (!hasHealthBar() && !hasHealthWidget()) {
			createHealthBar();
		}
		return true;
	}
	return false;
}

void UInventoryAndHealth::addItem(TObjectPtr<UItemInstance> item)
{
	Super::addItem( item);
	Health.CarriedWeight += item->ItemType->Weight * item->Quantity;
}

TObjectPtr<UItemInstance> UInventoryAndHealth::removeItem(TObjectPtr<UItemInstance> item, int quantity)
{
	TObjectPtr<UItemInstance> popped = Super::removeItem(item);
	Health.CarriedWeight -= popped->ItemType->Weight * popped->Quantity;
	return popped;
}

void UInventoryAndHealth::removeClothingItem(const UClothingItem* type, TObjectPtr<UItemInstance> item)
{
	Super::removeClothingItem(type, item);
	Health.Defence += type->Armor;
}

void UInventoryAndHealth::addClothingItem(const UClothingItem* type, TObjectPtr<UItemInstance> item)
{
	Super::addClothingItem(type, item);
	Health.Defence -= type->Armor;
}

bool UInventoryAndHealth::equipClothes(const UClothingItem* type, TObjectPtr<UItemInstance> owner)
{
	if (Health.IsFemale != type->IsFemale) return false;
	return Super::equipClothes(type, owner);
}

bool UInventoryAndHealth::onUnequipProjectile()
{
	return Super::onUnequipProjectile();
}

bool UInventoryAndHealth::onUnequipDoubleHanded()
{
	return Super::onUnequipDoubleHanded();
}

bool UInventoryAndHealth::onUnequipLeftHand()
{
	return Super::onUnequipLeftHand();
}

bool UInventoryAndHealth::onUnequipRightHand()
{
	return Super::onUnequipRightHand();
}

bool UInventoryAndHealth::onEquipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance> owner)
{
	return Super::onEquipProjectile(type, owner);
}

bool UInventoryAndHealth::onEquipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	return Super::onEquipDoubleHanded(type, owner);
}


bool UInventoryAndHealth::onEquipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	return Super::onEquipLeftHand(type, owner);
}

bool UInventoryAndHealth::onEquipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance> owner)
{
	return Super::onEquipRightHand(type, owner);
}

void UInventoryAndHealth::kill()
{
	
	Health.IsDead = true;
	Health.Health = 0;
	if (hasHealthBar()) {
		destroyHealthBar();
	}
	getPlayerMesh()->SetSimulatePhysics(true);
}

void UInventoryAndHealth::stripClothes()
{
	Super::stripClothes();
	Health.CarriedWeight = 0;
	Health.Defence = 0;
}

void UInventoryAndHealth::TickHealthBar()
{
	Health.HealthBar->Health->SetPercent(Health.getHealthPercentage());
}

bool UInventoryAndHealth::TickHealth(float DeltaTime)
{

	if (Health.IsDead) return false;

	if (Health.Health <= 0) {
		kill();
		return false;
	}
	Health.Tick(DeltaTime);
	if (Health.Widget) {
		Health.Widget->Stamina->SetPercent(Health.getStaminaPercentage());
		Health.Widget->Health->SetPercent(Health.getHealthPercentage());
		Health.Widget->Magic->SetPercent(Health.getMagicPercentage());
		Health.Widget->Arousal->SetPercent(Health.getArousalPercentage());
	}
	else if (Health.HealthBar) {
		TickHealthBar();
	}
	
	return true;
}


void UInventoryAndHealth::createHealthBar()
{
	check(IsValid(GetWorld()));
	if (HealthBarComponent == nullptr && IsValid(HealthBarClass)) {
		HealthBarComponent = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass());
		HealthBarComponent->SetWidgetClass(HealthBarClass);
		
		//HealthBarComponent->SetRelativeLocation(FVector(0, 0, 0));
		FVector origin;
		FVector extent;
		GetOwner()->GetActorBounds(true, origin, extent, false);
		FVector2D size(extent.X + extent.Y, 30+extent.Z*2);

		HealthBarComponent->SetDrawSize(size);
		check(IsValid(HealthBarComponent->GetWorld()));
		HealthBarComponent->InitWidget();
		HealthBarComponent->RegisterComponent();
		HealthBarComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);


		UNpcHealthBar* healthBar = Cast<UNpcHealthBar>(HealthBarComponent->GetWidget());
		healthBar->Health->SetPercent(Health.Health / Health.MaxHealth);
		Health.setHealthBar(healthBar);
	}
}

void UInventoryAndHealth::destroyHealthBar()
{
	if (HealthBarComponent != nullptr) {
		HealthBarComponent->DestroyComponent();
		HealthBarComponent = nullptr;
		Health.HealthBar = nullptr;
	}
}

void UInventoryAndHealth::TickHitDetection(float DeltaTime)
{
	if (invincibilityDuration > 0) {
		invincibilityDuration -= DeltaTime;
	}
	if (hitDetectionTimer > 0) {
		hitDetectionTimer -= DeltaTime;
	}
	else {
		DetectHit();
		hitDetectionTimer = HIT_DETECTION_PERIOD;
	}
}

void UInventoryAndHealth::DetectHit()
{
	AActor* actor = GetOwner();
	UWorld* world = GetWorld();
	TArray<FHitResult> OutHit;
	if (Right.HitDetectHand(actor, world, getPlayerMesh(), OutHit)) {
		const float damage = getTotalDamageRight();
		for (int i = 0; i < OutHit.Num(); i++) {
			IHittable* c = Cast< IHittable>(OutHit[i].GetActor());
			if (c)c->OnHit(this, RightHand, nullptr, damage);
		}
	}
	OutHit.Empty();
	if (Left.HitDetectHand(actor, world, getPlayerMesh(), OutHit)) {
		const float damage = getTotalDamageLeft();
		for (int i = 0; i < OutHit.Num(); i++) {
			IHittable* c = Cast< IHittable>(OutHit[i].GetActor());
			if (c)c->OnHit(this, LeftHand, nullptr, damage);
		}
	}
}