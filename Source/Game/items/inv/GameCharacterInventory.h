// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatInventory.h"
#include "../../character/designer/Race.h"
#include "GameCharacterInventory.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UGameCharacterInventory  : public UCombatInventory
{
	GENERATED_BODY()

	

public:	

	/** The male skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URace> Race;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	FLinearColor HairColor = FLinearColor(0., 0., 1.);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	int HairdoIndex = -1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	FName HairSocket;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> HairMeshComponent;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicHairMaterial = nullptr;

	////////////////////////////////////////
	/////////////// GETTERS ////////////////
	////////////////////////////////////////
	
	inline const FGender& GetGender() const{
		return Race->getGender(IsFemale());
	}
	inline void SetGender(const  bool isFemale) {
		if (isFemale != IsFemale()) {
			USkeletalMeshComponent* mesh = getPlayerMesh();
			mesh->SetSkeletalMesh(Race->getBodyMesh(isFemale));
			mesh->SetAnimInstanceClass(Race->getAnimClass(isFemale));
			Health.IsFemale = isFemale;
		}
	}
	inline bool hasHair() const{
		return HairdoIndex >= 0;
	}
	inline FText getHairdoName() const {
		if (HairdoIndex < 0)return FText::FromString("Bald");
		return GetGender().Hairdos[HairdoIndex].Name;
	}
	inline int getHairdo() const {
		return HairdoIndex;
	}
	void setHairdo(int index);
	inline FLinearColor getHairColor() {
		return HairColor;
	}
	inline void setHairColor(FLinearColor rgb) {
		HairColor = rgb;
		if (DynamicHairMaterial != nullptr) {
			DynamicHairMaterial->SetVectorParameterValue("color", rgb);
		}
	}

	virtual void setPlayerMesh(USkeletalMeshComponent* playerMesh) override;
	////////////////////////////////////////
	/////////////// TICKS   ////////////////
	////////////////////////////////////////

	inline void TickEverything(float DeltaTime) {
		TickCombat(DeltaTime);
	}

};
