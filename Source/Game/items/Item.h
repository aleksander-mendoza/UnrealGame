// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemClass.h"
#include "Item.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct GAME_API FItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Armor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Warmth=0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Durbility=0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value=0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage=0;

	// this is a bitfield.
	// 00000000000000000000000000000000 = 0 = not a clothing item (can't be worn)
	// 00000000000000000000000000000001 = 1 = stomach
	// 00000000000000000000000000000010 = 2 = head
	// 00000000000000000000000000000100 = 4 = panty
	// 00000000000000000000000000001000 = 8 = neck
	// 00000000000000000000000000010000 = 16 = shoulders
	// 00000000000000000000000000100000 = 32 = left foot 
	// 00000000000000000000000001000000 = 64 = right foot 
	// 00000000000000000000000010000000 = 128 = left hand
	// 00000000000000000000000100000000 = 256 = right hand
	// 00000000000000000000001000000000 = 512 = left thigh
	// 00000000000000000000010000000000 = 1024 = right thigh
	// 00000000000000000000100000000000 = 2048 = left shin
	// 00000000000000000001000000000000 = 4096 = right shin
	// 00000000000000000010000000000000 = 8192 = left forearm
	// 00000000000000000100000000000000 = 16384 = right forearm  
	// 00000000000000001000000000000000 = 32768 = left breast 
	// 00000000000000010000000000000000 = 65536 = right breast
	// 00000000000000100000000000000000 = 131072 = back
	// 00000000000001000000000000000000 = 262144 = left ring
	// 00000000000010000000000000000000 = 524288 = right ring
	// 00000000000100000000000000000000 = 1048576 = left bracelet
	// 00000000001000000000000000000000 = 2097152 = right bracelet
	// 00000000010000000000000000000000 = 4194304 = wing
	// 00000000100000000000000000000000 = 8388608 = hood
	// 00000001000000000000000000000000 = 16777216 = face
	// 00000010000000000000000000000000 = 33554432 = 
	// 00000100000000000000000000000000 = 67108864
	// 00001000000000000000000000000000 = 134217728
	// 00010000000000000000000000000000 = 268435456
	// 00100000000000000000000000000000 = 536870912
	// 01000000000000000000000000000000 = 1073741824
	// 10000000000000000000000000000000 = -2147483648 = can be freely combined without colliding
	// here are some useful combinations
	// 128+8192 = 8320 = left arm (hand+forearm)
	// 256+16384 = 16640 = right arm (hand+forearm)
	// 32+512+2048 = 2592 = left leg (foot+shin+thigh)
	// 64+1024+4096 = 5184 = right leg (foot+shin+thigh)
	// 4+32+512+2048+64+1024+4096 = 7780 = full bottom (foot+shin+thigh+panties)
	// 4+512+2048+1024+4096 = 7684 = bottom without feet (shin+thigh+panties)
	// 32768+65536 = 98304 = breasts 
	// 16+128+8192+256+16384 = 24976 = both arms (hand+forearm+shoulders)
	// 1+32768+65536+131072 = 229377 = torso without shoulders (breasts+stomach+back)
	// 1+16+32768+65536+131072 = 229393 = full torso (breasts+stomach+back+shoulders)
	// 1+32768+65536+131072+4 = 229381 = leotard (breasts+stomach+back+panties)
	// 1+16+32768+65536+131072+128+8192+256+16384 = 254353 = upper body (breasts+stomach+back+shoulders+hand+forearm)

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ClothingSlots=0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnchantmentSlots=0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Level=0;

	/**Devious devices cannot be uneqipped without a key/lockpick/struggle. The higher the level the more difficult it is to escape.
	Level 0 means the item is not devious (can be freely unequipped). Level 1,2,3... requires keys of the corresponding level. 
	Level -1, -2, -3 cannot be picked up or struggled out of and must be opened with corresponding key 1,2,3...*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int DeviousLevel = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemClass Class= EItemClass::PROP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight=0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> WearableMesh;

	inline bool isWearable() const {
		check((ClothingSlots != 0) == ::isWearable(Class));
		return ClothingSlots!=0;
	}
	inline bool isDevious() const {
		return DeviousLevel != 0;
	}
	inline bool conflictsWith(const FItem& other) const {
		return (other.ClothingSlots & ClothingSlots) != 0;
	}
	inline bool hardConflictsWith(const FItem& other) const {
		return isDevious() && conflictsWith(other);
	}
	inline USkeletalMesh* getSkeletalMesh() {
		return WearableMesh.LoadSynchronous();
	}
	inline UStaticMesh* getMesh() {
		return Mesh.LoadSynchronous();
	}
	inline bool isDoubleHanded() const {
		return ::isDoubleHanded(Class);
	}
	inline bool isSheathedOnTheBack() {
		return ::isSheathedOnTheBack(Class);
	}
	inline bool isSkeletal() {
		return Mesh.IsNull();
	}
	/**If the weapon is double-handed then one of the hands is dominant. 
	It determines which socket the mesh is actually attached to. 
	So if animation requires the weapon to be temporarily held in just one hand,
	that is the hand that will hold it. Bows are held in left hand. 
	Great swords are held in right hand.*/
	inline bool isLeftTheDominantHand() {
		return ::isLeftTheDominantHand(Class);
	}
};
