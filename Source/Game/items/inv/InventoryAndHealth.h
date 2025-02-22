// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshedInventory.h"
#include "Health.h"
#include "Components/WidgetComponent.h"
#include "InventoryAndHealth.generated.h"

#define HIT_DETECTION_PERIOD 0.1
//DECLARE_DELEGATE_OneParam(FStringDelegate, FString);
class UNpcHealthBar;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UInventoryAndHealth : public UMeshedInventory
{
	GENERATED_BODY()

public:	


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarComponent;

	/* The AnimBlueprint class to use. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UNpcHealthBar> HealthBarClass;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FHealth Health;


	////////////////////////////////////////
	/////////////// GETTERS ////////////////
	////////////////////////////////////////

	bool hasHealthWidget() const{
		return Health.Widget != nullptr;
	}

	bool hasHealthBar() const {
		return HealthBarComponent != nullptr;
	}
	void createHealthBar();
	void destroyHealthBar();

	inline float getTotalDamageLeft() {
		return LeftHand == nullptr ? Health.BareHandDamage : LeftHand->getTotalDamage(this);
	}
	inline float getTotalDamageRight() {
		return RightHand == nullptr ? Health.BareHandDamage : RightHand->getTotalDamage(this);
	}
	inline bool IsDead() const {
		return Health.IsDead;
	}
	inline bool IsFemale() const {
		return Health.IsFemale;
	}
	inline bool CanAttack() const {
		return Health.CanAttack();
	}
	inline bool CanJump() const {
		return Health.CanJump();
	}
	inline bool CanRun() const {
		return Health.CanRun();
	}
	////////////////////////////////////////
	/////////////// EVENTS ////////////////
	////////////////////////////////////////

	virtual bool onHit(UInventoryAndHealth* attacker, class UItemInstance* weaponUsed, class UItemInstance* projectile, float damage) override;

	virtual void addItem(TObjectPtr<UItemInstance> item) override;

	virtual TObjectPtr<UItemInstance> removeItem(TObjectPtr<UItemInstance> item, int quantity) override;

	virtual void removeClothingItem(const class UClothingItem* type, TObjectPtr < UItemInstance > item) override;

	virtual void addClothingItem(const class UClothingItem* type, TObjectPtr < UItemInstance> item) override;

	virtual bool equipClothes(const class UClothingItem* type, TObjectPtr < UItemInstance > owner);

	virtual bool onUnequipProjectile() override;

	virtual bool onUnequipDoubleHanded() override;

	virtual bool onUnequipLeftHand() override;

	virtual bool onUnequipRightHand() override;

	virtual bool onEquipProjectile(const UProjectileItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual bool onEquipDoubleHanded(const UDoubleHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual bool onEquipLeftHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual bool onEquipRightHand(const UOneHandedWeaponItem* type, TObjectPtr<UItemInstance>  owner) override;

	virtual void kill();

	virtual void stripClothes() override; 
	

	////////////////////////////////////////
	/////////////// TICKS   ////////////////
	////////////////////////////////////////

	void DetectHit();

	void TickHitDetection(float DeltaTime);

	virtual void TickHealthBar();

	bool TickHealth(float DeltaTime);
};
