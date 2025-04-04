// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActorStatic.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ItemActorProjectile.generated.h"

class UInventoryAndHealth;

UCLASS()
class GAME_API AItemActorProjectile : public AItemActorStatic
{
	GENERATED_BODY()
	


	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> Movement;


	UPROPERTY()
	UInventoryAndHealth* Shooter;
	
	UPROPERTY()
	TObjectPtr<UItemInstance> RangedWeapon;

public:	
	// Sets default values for this actor's properties
	AItemActorProjectile();

	UFUNCTION()
	void onProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual bool isProjectile() const override {
		return true;
	}
	inline void shoot(UInventoryAndHealth* shooter, TObjectPtr<UItemInstance> rangedWeapon, FVector velocity) {
		Shooter = shooter;
		RangedWeapon = rangedWeapon;
		Movement->Velocity = velocity;
		Movement->SetUpdatedComponent(RootComponent);
		Mesh->OnComponentHit.AddUniqueDynamic(this, &AItemActorProjectile::onProjectileHit);
		
	}
	

};
