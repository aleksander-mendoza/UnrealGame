// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "../blender/utildefines.h"
#include "../items/ItemActorStatic.h"
#include "../items/ItemActorProjectile.h"
#include "../items/ItemActorSkeletal.h"
#include "../GameCharacter.h"
#include "OpenWorld.generated.h"


UCLASS()
class GAME_API AOpenWorld : public AActor
{
	GENERATED_BODY()

	// Sets default values for this actor's properties
	AOpenWorld();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true", RowType = "Hairdo"))
	UDataTable * Hairdos;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<AGameCharacter> PlayerPawnClass;

	

	void PostInitializeComponents() override;

	UPROPERTY()
	TArray<AItemActorProjectile*> usedProjectileActorPool;
	UPROPERTY()
	TArray<AItemActorProjectile*> unusedProjectileActorPool;

	UPROPERTY()
	TArray<AItemActorStatic*> usedStaticActorPool;
	UPROPERTY()
	TArray<AItemActorStatic*> unusedStaticActorPool;

	UPROPERTY()
	TArray<AItemActorSkeletal*> usedSkeletalActorPool;
	UPROPERTY()
	TArray<AItemActorSkeletal*> unusedSkeletalActorPool;

	UPROPERTY()
	TArray<AGameCharacter*> usedNPCPool;
	UPROPERTY()
	TArray<AGameCharacter*> unusedNPCPool;


	template<typename T>
	inline T* spawnActor(TArray<T*> & usedPool, TArray<T *> & unusedPool, double3 loc, FRotator rot, UClass * clazz) {
		T* a;
		/*for (int i = 0; i < unusedPool.Num(); i++) {
			a = unusedPool[i];
			check(!a->isEnabled());
			if (a->GetMesh() == mesh) {
				unusedPool.RemoveAtSwap(i, EAllowShrinking::No);
				a->SetActorLocation(loc);
				a->SetActorRotation(rot);
				goto end;
			}
		}*/

		if (!unusedPool.IsEmpty()) {
			a = unusedPool.Pop();
			a->SetActorLocation(loc);
			a->SetActorRotation(rot);
		}
		else {
			a = GetWorld()->SpawnActor<T>(clazz, loc, rot);
		}
	//end:
		usedPool.Add(a);
		return a;
	}
	template<typename T>
	inline void despawnActor(TArray<T*>& usedPool, TArray<T*>& unusedPool, T* actor) {
		usedPool.RemoveSingleSwap(actor);
		unusedPool.Add(actor);
		actor->SetEnabled(false);
	}
public:
	UPROPERTY()
	AGameCharacter* PlayerPawn;

	UDataTable* getHairdos() const{
		return Hairdos;
	}
	void despawnItemStatic(AItemActorStatic * item) {
		if (item->isProjectile()) {
			despawnItemProjectile((AItemActorProjectile*)item);
		}
		else {
			despawnActor(usedStaticActorPool, unusedStaticActorPool, item);
		}
	}
	AItemActorStatic* spawnItemStatic(UItemObject* item, double3 loc,FRotator rot) {
		TObjectPtr<AItemActorStatic> a = spawnActor(usedStaticActorPool, unusedStaticActorPool, loc, rot, AItemActorStatic::StaticClass());
		a->setItem(item);
		a->worldRef = this;
		return a;
	}
	void despawnItemProjectile(AItemActorProjectile * item) {
		despawnActor(usedProjectileActorPool, unusedProjectileActorPool, item);
	}
	AItemActorProjectile* spawnItemProjectile(UItemObject* item, double3 loc, FRotator rot) {
		AItemActorProjectile * a = spawnActor(usedProjectileActorPool, unusedProjectileActorPool, loc, rot, AItemActorProjectile::StaticClass());
		a->setItem(item);
		a->worldRef = this;
		return a;
	}
	void despawnItemSkeletal(AItemActorSkeletal * item) {
		despawnActor(usedSkeletalActorPool, unusedSkeletalActorPool, item);
	}
	AItemActorSkeletal * spawnItemSkeletal(UItemObject * item, double3 loc, FRotator rot) {
		AItemActorSkeletal * a = spawnActor(usedSkeletalActorPool, unusedSkeletalActorPool, loc, rot, AItemActorSkeletal::StaticClass());
		a->setItem(item);
		a->worldRef = this;
		return a;
	}


	void despawnItem(AItemActor * item) {
		if (item->isSkeletal()) {
			despawnItemSkeletal((AItemActorSkeletal*)item);
		}
		else {
			despawnItemStatic((AItemActorStatic*)item);
		}
	}
	AItemActor * spawnItem(UItemObject * item, double3 loc, FRotator rot) {
		if (item->Instance.getRow()->isSkeletal()) {
			return spawnItemSkeletal(item, loc, rot);
		}
		else {
			return spawnItemStatic(item, loc, rot);
		}
	}

	void despawnNPC(AGameCharacter * npc) {
		despawnActor(usedNPCPool, unusedNPCPool, npc);
	}
	AGameCharacter * spawnNPC(AGameCharacter * npc, double3 loc, FRotator rot) {
		AGameCharacter * a = spawnActor(usedNPCPool, unusedNPCPool, loc, rot, PlayerPawnClass);
		a->worldRef = this;
		return a;
	}
	inline const double3 getPlayerPos() {
		if (PlayerPawn == nullptr)return double3(0, 0, 0);
		return PlayerPawn->GetActorLocation();
	}
	inline AItemActorProjectile* shootProjectile(AGameCharacter * shooter, UItemObject * projectile, double speed) {
		FRotator rot = shooter->GetBaseAimRotation();
		FVector loc = shooter->getHeadLocation() + rot.Vector() * 100;
		//FVector loc(0, 0, 400);
		AItemActorProjectile * item = spawnItemProjectile(projectile, loc, rot);
		UItemObject * weapon = shooter->Inventory->LeftHand;
		item->shoot(shooter, weapon, rot.Vector() * speed);
		return item;
	}


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every Entity
	virtual void Tick(float DeltaTime) override;


};