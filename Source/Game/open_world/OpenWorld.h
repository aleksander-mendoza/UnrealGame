// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "../blender/utildefines.h"
#include "../items/ItemActor.h"
#include "../GameCharacter.h"
#include "OpenWorld.generated.h"


UCLASS()
class GAME_API AOpenWorld : public AActor
{
	GENERATED_BODY()

	// Sets default values for this actor's properties
	AOpenWorld();
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<AGameCharacter> PlayerPawnClass;

	UPROPERTY()
	AGameCharacter* PlayerPawn;

	void PostInitializeComponents() override;

	UPROPERTY()
	TArray<TObjectPtr<AItemActor>> usedStaticActorPool;
	UPROPERTY()
	TArray<TObjectPtr<AItemActor>> unusedStaticActorPool;


	UPROPERTY()
	TArray<TObjectPtr<AGameCharacter>> usedNPCPool;
	UPROPERTY()
	TArray<TObjectPtr<AGameCharacter>> unusedNPCPool;


	template<typename T>
	inline TObjectPtr<T> spawnActor(TArray<TObjectPtr<T>> & usedPool, TArray<TObjectPtr<T>> & unusedPool, double3 loc, FRotator rot, UClass * clazz) {
		TObjectPtr<T> a;
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
	inline void despawnActor(TArray<TObjectPtr<T>>& usedPool, TArray<TObjectPtr<T>>& unusedPool, TObjectPtr<T> actor) {
		usedPool.RemoveSingleSwap(actor);
		unusedPool.Add(actor);
		actor->SetEnabled(false);
	}
public:
	void despawnItem(TObjectPtr<AItemActor> item) {
		despawnActor(usedStaticActorPool, unusedStaticActorPool, item);
	}
	TObjectPtr<AItemActor> spawnItem(TObjectPtr<UItemObject> item, double3 loc,FRotator rot) {
		TObjectPtr<AItemActor> a = spawnActor(usedStaticActorPool, unusedStaticActorPool, loc, rot, AItemActor::StaticClass());
		UStaticMesh* mesh = item->getMesh();
		a->setItem(item, mesh);
		return a;
	}
	void despawnNPC(TObjectPtr<AGameCharacter> item) {
		despawnActor(usedNPCPool, unusedNPCPool, item);
	}
	TObjectPtr<AGameCharacter> spawnNPC(TObjectPtr<AGameCharacter> npc, double3 loc, FRotator rot) {
		TObjectPtr<AGameCharacter> a = spawnActor(usedNPCPool, unusedNPCPool, loc, rot, PlayerPawnClass);
		a->worldRef = this;
		return a;
	}
	inline const double3 getPlayerPos() {
		if (PlayerPawn == nullptr)return double3(0, 0, 0);
		return PlayerPawn->GetActorLocation();
	}
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every Entity
	virtual void Tick(float DeltaTime) override;


};