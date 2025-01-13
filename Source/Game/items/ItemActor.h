// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemObject.h"
#include "../character/Interactable.h"
#include "ItemActor.generated.h"


UCLASS()
class GAME_API AItemActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UItemObject> Item;

	/** The female skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	
public:	
	// Sets default values for this actor's properties
	AItemActor();

	virtual bool OnInteract(class AGameCharacter* actor);
	
	inline bool isEnabled() const{
		return this->IsHidden();
	}
	inline void SetEnabled(bool enabled) {
		this->SetActorHiddenInGame(!enabled);
		this->SetActorEnableCollision(enabled);
	}
	inline void SetMesh(UStaticMesh* mesh){
		if (mesh != nullptr) {
			Mesh->SetStaticMesh(mesh);
		}
		SetEnabled(mesh != nullptr);
		
	}
	TObjectPtr<UStaticMesh> GetMesh() {
		return Mesh->GetStaticMesh();
	}
	inline void setItem(TObjectPtr<UItemObject> item) {
		return setItem(item, item->getMesh());
	}
	inline void setItem(TObjectPtr<UItemObject> item, UStaticMesh * mesh) {
		check(item->getMesh() == mesh);
		Item = item;
		SetMesh(mesh);
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
