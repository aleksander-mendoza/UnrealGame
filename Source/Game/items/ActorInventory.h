// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemObject.h"
#include "ItemInstanceList.h"
#include "ActorInventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UActorInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorInventory();

	UPROPERTY()
	TArray<UItemObject*> Items;


	/**These are items that are currently worn as clothes*/
	UPROPERTY()
	TArray<UItemObject*> Clothes;

	/**These is the item that is currently held in left hand*/
	UPROPERTY()
	UItemObject* LeftHand;

	/**These is the item that is currently held in right hand (it could be the same object as in left hand. This is the case for double-handed weapons)*/
	UPROPERTY()
	UItemObject* RightHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FItemInstanceList DefaultItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FItemInstanceList DefaultClothes;

	virtual void PostInitProperties() override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ResetToDefaultItems();
	void ResetToDefaultClothes();
	inline void ResetToDefault() {
		ResetToDefaultItems();
		ResetToDefaultClothes();
	}
		
};
