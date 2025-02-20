// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActorProjectile.h"
#include "../../open_world/OpenWorld.h"

// Sets default values
AItemActorProjectile::AItemActorProjectile()
{
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	//Movement->bShouldBounce = true;
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetSimulatePhysics(false);
	Movement->ProjectileGravityScale = 0.1;// 1.0;
	//Movement->Bounciness = 0.1f;
	Movement->Friction = 0.5f;
	Movement->InitialSpeed = 1000.0;
	Movement->UpdatedComponent = RootComponent;
	

}

void AItemActorProjectile::onProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	if (IHittable* hittable = Cast<IHittable>(OtherActor)) {
		
		double hitSpeed = this->GetVelocity().Length();
		float bowDmgMultiplier = Shooter->Health.BowDamageMultiplier;
		float bowDmg = RangedWeapon->getDamage();
		float arrowDmg = Item->getDamage();
		float totalDmg = (bowDmg + arrowDmg) * hitSpeed / 1000. * bowDmgMultiplier;
		hittable->OnHit(Shooter, RangedWeapon, Item, totalDmg);
		worldRef->despawnItemProjectile(this);
	}
	Mesh->OnComponentHit.Clear();
}
