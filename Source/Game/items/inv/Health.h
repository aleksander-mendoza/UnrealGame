// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../ui/Status.h"
#include "../perks/Perk.h"
#include "../../anim/ArmedPoseType.h"
#include "Health.generated.h"

class AGameCharacter;
class UNpcHealthBar;

USTRUCT(BlueprintType)
struct GAME_API FHealth
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UPerk *> Perks;

	////////////////////////////////////////////////
	//////////////// SURVIVAL         //////////////
	////////////////////////////////////////////////

	
	
	float PreventStaminaRegen = 0;
	float PreventHealthRegen = 0;
	float PreventMagicRegen = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsDead = false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Stamina = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxStamina=100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StaminaRegenSpeed=20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HealthRegenSpeed=5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Warmth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxWarmth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Magic = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxMagic = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MagicRegenSpeed=20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dirtiness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Defence = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Alcohol;

	float getStaminaPercentage() { return Stamina / MaxStamina; }
	float getHealthPercentage() { return Health / MaxHealth; }
	float getMagicPercentage() { return Magic / MaxMagic; }
	float getArousalPercentage() { return Arousal / 100.; }

	////////////////////////////////////////////////
	//////////////// MOVEMENT         //////////////
	////////////////////////////////////////////////


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WalkSpeed = 200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SlowWalkSpeed = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RunSpeed = 500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CarriedWeight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxCarriedWeight = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LightArmorMultiplier = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeavyArmorMultiplier = 1;


	UPROPERTY( EditAnywhere, BlueprintReadOnly)
	float BowAttackCooldown = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SingleHandedAttackCooldown = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DoubleHandedAttackCooldown = 0.2;

	float GetAttackCooldown(EArmedPoseType pose) {
		switch (pose) {
		case EArmedPoseType::DOUBLE_HANDED:
			return DoubleHandedAttackCooldown;
		case EArmedPoseType::BARE_HANDED:
		case EArmedPoseType::SINGLE_HANDED:
		case EArmedPoseType::UNARMED:
			return SingleHandedAttackCooldown;
		case EArmedPoseType::BOW_AIMED:
		case EArmedPoseType::BOW:
			return BowAttackCooldown;
		default:
			check(false);
			return SingleHandedAttackCooldown;
		}
	}
	////////////////////////////////////////////////
	//////////////// ABILITIES        //////////////
	////////////////////////////////////////////////

	inline bool HasBoundHands() const {
		return false;
	}

	inline bool HasBoundLegs() const {
		return false;
	}

	inline bool CanAttack() const{
		return !HasBoundHands();
	}

	inline bool CanJump() const {
		return true;
	}

	inline bool IsCarryingTooMuch() const {
		return CarriedWeight > MaxCarriedWeight;
	}
	inline bool CanRun() const {
		return !(HasBoundLegs() || IsCarryingTooMuch());
	}
	inline float GetWalkSpeed() const {
		return CanRun() ? WalkSpeed : SlowWalkSpeed;
	}

	inline float GetRunSpeed() const {
		return CanRun() ? RunSpeed : SlowWalkSpeed;
	}
	////////////////////////////////////////////////
	//////////////// DAMAGE           //////////////
	////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BareHandDamage = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SingleHandedWeaponDamageMultiplier = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DoubleHandedWeaponDamageMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StealthDamageMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BowDamageMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpearDamageMultiplier = 1;



	////////////////////////////////////////////////
	//////////////// MOOD             //////////////
	////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Arousal=10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ArousalRegenSpeed=0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Anger;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Happiness;


	////////////////////////////////////////////////
	//////////////// SKILLS               //////////
	////////////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DestructionMagicSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RestorationMagicSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IllusionMagicSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AlterationMagicSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SneakSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SingleHandedSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DoubleHandedSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BowSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TradingSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DiplomacySkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PickpocketSkill;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float OralSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AnalSkill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float VaginalSkill;
	/**You find it easier to find clients and lowers the chances of getting assaulted. 
	You need at this perk at level at least 1 in order to be able to prostitute yourself.
	To gain this perk you need to talk with tavern owners and find someone who decides to hire you.
	You can always try to sell yourself on your own but this the chances of getting assaulted, not getting paid 
	or getting turned over to guards are prohibitively high. Prostitution on your own is illegal without a license.
	Public sex is illegal everywhere (in towns/villages. Technically in the wilderness nobody cares 
	but there are also hardly any clients to find)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ProstitutionSkill;
	/**Increases effectivness of flirting. Once you reach certain seduction level with some NPC, you will be able to fuck them.
	This field represents the seduction skill if this is a player character. If this is an NPC character then this field
	represents affection level towards the player character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SedcutionSkill;
	/**Reanimating corpses becomes more effective. You need at this perk at level at least 1 in order to be able to reanimate corpses.
	To gain this perk you need to study necromancy under a more experienced necromancer (at least level 5).*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 NecromancerPerk;
	/**You need this perk before you can unlock other succubus perks. Obtaining this perk can be done by first becoming 
	a vampire and then performing a special ritual.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 SuccubusPerk;
	/**You gain this perk when you become a vampire. To become a vampire you have to seduce one and THEN get bit. 
	If you get bit before seducing they could turn you into a vampire thrall (a slave). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 VampirePerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 DoubleHandedPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 SingleHandedPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/**Improves pickopet chances*/
	uint8 ThiefPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/**You can steal people's panties*/
	uint8 PantyThiefPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/**You can steal people's bras*/
	uint8 BraThiefPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/**You can steal any equipped clothing item*/
	uint8 ClothesThiefPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/**You can use love potion brewing stand */
	uint8 LovePotionAlchemist;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/**You can use smithing stand*/
	uint8 SmithingPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/**You can use ore smelting stand*/
	uint8 SmeltingPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/**You can use loom */
	uint8 LoomPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/**You can higher chance of quietly assasinating enemies when sneaking*/
	uint8 AssassinPerk;
	/**Bows deal mor edamage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 ArcherPerk;
	/**Brewed potions are stronger */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 AlchemistPerk;
	/**Using sledge hammers deals more damage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 HammerPerk;
	/**Using great swords deals more damage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 GreatswordPerk;
	/**Using daggers deals more damage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 DaggerPerk;
	/**Using fire magic deals more damage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 FirePerk;
	/**Using lightning magic deals more damage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 LightningPerk;
	/**Slaves don't gain their own experience. Instead their experience increases when their master levels-up. 
	Higher slave perk results in you getting higher percentage of your master's experience. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 SlavePerk;
	/**Making new slaves is easier*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 SlaverPerk;
	/**Traning male slaves is easier*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 MaleSlaverPerk;
	/**Traning female slaves is easier*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 FemaleSlaverPerk;
	/**Succubus can absorb stamina/heath/magic of their prey*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 SuccubusStaminaDrainPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 SuccubusHealthDrainPerk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 SuccubusMagicDrainPerk;
	/**Succubus can kill the person they have sex with*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 SuccubusKillPerk;
	/**Succubus can seduce others more effectively*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 SuccubusSedcutionPerk;
	

	////////////////////////////////////////////////
	//////////////// PERSONALITY          //////////
	////////////////////////////////////////////////
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Adventurer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Diplomat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Entrepreneur;


	////////////////////////////////////////////////
	//////////////// SEX PROPERTIES //////////////
	////////////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsFemale;
	
	int8 LikesVaginal;
	
	int8 LikesOral;
	
	int8 LikesAnal;
	
	int8 LikesMales;
	
	int8 LikesFemales;
	
	int8 LikesAss;
	
	int8 LikesTits;
	
	int8 LikesMuscles;


	////////////////////////////////////////////////
	//////////////// SLAVE PROPERTIES //////////////
	////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Submission;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float OralTraining;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float VaginalTraining;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AnalTraining;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BondageTraining;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WhipTraining;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Insults;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Compliments;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PublicShaming;



	TObjectPtr<UNpcHealthBar> HealthBar;
	TObjectPtr<UStatus> Widget=nullptr;

	void setHealthBar(TObjectPtr<UNpcHealthBar> widget) {
		HealthBar = widget;
	}
	void setWidget(TObjectPtr<UStatus> widget) {
		Widget = widget;
		//Widget->healthComponenet = this;
	}

	void Tick(float DeltaTime);
};
