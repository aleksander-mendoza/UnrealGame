// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ItemInstance.h"
#include "../../character/Interactable.h"
#include "../../character/Hittable.h"
#include "ItemActor.generated.h"


UCLASS()
class GAME_API AItemActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
	


public:	
	// Sets default values for this actor's properties
	AItemActor();

	class AOpenWorld* worldRef;

	UPROPERTY()
	TObjectPtr<UItemInstance> Item;

	

	virtual bool isSkeletal() const{
		return false;
	}
	virtual void setItem(TObjectPtr<UItemInstance> item) {
		Item = item;
		SetEnabled(item != nullptr);
	}

	virtual bool OnInteract(class AGameCharacter* actor);
	
	inline bool isEnabled() const{
		return this->IsHidden();
	}
	inline void SetEnabled(bool enabled) {
		this->SetActorHiddenInGame(!enabled);
		this->SetActorEnableCollision(enabled);
	}
	
};
