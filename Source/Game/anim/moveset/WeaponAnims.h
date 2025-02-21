// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAnim.h"
#include "../ArmedPoseType.h"
#include "WeaponAnims.generated.h"

class UGameCharacterInventory;
/**
 * 
 */
UCLASS(BlueprintType)
class GAME_API UWeaponAnims : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UWeaponAnims() {
		LeftUnsheathSocket = TEXT("hand_lSocket");
		RightUnsheathSocket = TEXT("hand_rSocket");
		LeftSheathSocket = TEXT("sheathed_lSocket");
		RightSheathSocket = TEXT("sheathed_rSocket");
		SheathedHandleSocket = TEXT("Handle");
		UnsheathedHandleSocket = TEXT("Handle");
	}
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FWeaponAnim Sheath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FWeaponAnim Unsheath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FWeaponAnim> AttackAnims;

	/** Hit Animation*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FWeaponAnim HitAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName LeftUnsheathSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName RightUnsheathSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName LeftSheathSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName RightSheathSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SheathedHandleSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UnsheathedHandleSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EArmedPoseType ArmedPose = EArmedPoseType::SINGLE_HANDED;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsLeftTheDominantHand=false;


	inline const FName& getSheathedSocketName(bool leftHand) const{
		return leftHand?LeftSheathSocket:RightSheathSocket;
	}
	inline const FName& getUnsheathedSocketName(bool leftHand) const {
		return leftHand ? LeftUnsheathSocket : RightUnsheathSocket;
	}

	const FWeaponAnim * GetAnim() const {
		if (AttackAnims.Num() > 0) {
			int idx = FMath::RandRange(0, AttackAnims.Num()-1);
			const FWeaponAnim& anim = AttackAnims[idx];
			check(IsValid(anim.Anim));
			return &anim;
		}
		return nullptr;
	}

	inline const UWeaponAnims* max(const UWeaponAnims* other) const {
		check(other != nullptr);
		return uint8(ArmedPose) < uint8(other->ArmedPose) ? other : this;
	}

	virtual bool startAttack(UGameCharacterInventory* user, bool leftHand) const;
	virtual void endAttack(UGameCharacterInventory* user, bool leftHand) const;
	virtual void cancelAttack(UGameCharacterInventory* user) const;

	
};
