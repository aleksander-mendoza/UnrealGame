// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActor.h"
#include "ItemActorSkeletal.generated.h"


UCLASS()
class GAME_API AItemActorSkeletal : public AItemActor
{
	GENERATED_BODY()
	

	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;


public:	
	// Sets default values for this actor's properties
	AItemActorSkeletal();

	inline USkeletalMeshComponent* getMeshComponent() {
		return Mesh;
	}
	virtual bool isSkeletal() const override {
		return true;
	}

	virtual void setItem(TObjectPtr<UItemObject> item) override{
		Super::setItem(item);
		if (item) {
			FItem* row = item->getRow();
			check(row->isSkeletal());
			Mesh->SetSkeletalMesh(row->getSkeletalMesh());
		}
		
	}


};
