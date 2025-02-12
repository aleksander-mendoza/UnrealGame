// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../ui/Status.h"
#include "../items/ItemObject.h"
//#include "HealthEvents.h"
#include "Health.generated.h"

class AGameCharacter;

USTRUCT(BlueprintType)
struct GAME_API FHealth
{
	GENERATED_BODY()

	// Sets default values for this component's properties
	FHealth();


	////////////////////////////////////////////////
	//////////////// SURVIVAL         //////////////
	////////////////////////////////////////////////

	
	
	float PreventStaminaRegen = 0;
	float PreventHealthRegen = 0;
	float PreventMagicRegen = 0;

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

	inline float getHealthPercentage() {
		return Health / MaxHealth;
	}
	void setStamina(float stamina) {
		if (stamina > MaxStamina) stamina = MaxStamina;
		if (Widget)Widget->Stamina->SetPercent(stamina / MaxStamina);
		Stamina = stamina;
	}

	bool takeStamina(float stamina) {
		if (Stamina >= stamina) {
			setStamina(Stamina - stamina);
			return true;
		}
		return false;
	}

	void setHealth( AGameCharacter* character, float hp);

	void takeHealth( AGameCharacter* character, float health) {
		check(health >= 0);
		setHealth(character, Health - health);
	}
	bool damage(AGameCharacter* character, AGameCharacter* attacker, float health);
	void kill( AGameCharacter* character,  AGameCharacter* killer);
	void heal( AGameCharacter* character, float health);

	void setMagic(float m) {
		if (m > MaxMagic) m = MaxMagic;
		if (Widget)Widget->Stamina->SetPercent(m / MaxMagic);
		Magic = m;
	}

	bool takeMagic(float m) {
		if (Magic >= m) {
			setMagic(Magic - m);
			return true;
		}
		return false;

	}


	////////////////////////////////////////////////
	//////////////// MOVEMENT         //////////////
	////////////////////////////////////////////////

	bool UpdateRunning(float DeltaTime) {
		float s = Stamina - 10 * DeltaTime;
		PreventStaminaRegen = 1;
		if (s < 0) {
			s = 0;

		}
		setStamina(s);
		return s <= 0;
	}
	inline bool CanRun() {
		return Stamina > 10;
	}
	inline bool CanJump() {
		return true;
	}




	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CarriedWeight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxCarriedWeight = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LightArmorMultiplier = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeavyArmorMultiplier = 1;


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


	float getDamage(const UItemObject* item) {
		if (item == nullptr) {
			return BareHandDamage;
		}
		return getDamage(item->Instance);
	}

	float getDamage(const FItemInstance& item) {
		const FItem* row = item.getRow();
		switch (row->Class) {
		case EItemClass::DOUBLE_HANDED_WEAPON:
			return row->Damage * DoubleHandedWeaponDamageMultiplier;
		case EItemClass::SINGLE_HANDED_QUIET_WEAPON:
			return row->Damage * StealthDamageMultiplier;
		case EItemClass::SINGLE_HANDED_WEAPON:
			return row->Damage * SingleHandedWeaponDamageMultiplier;
		case EItemClass::BOW:
			return row->Damage * BowDamageMultiplier;
		case EItemClass::SPEAR:
			return row->Damage * SpearDamageMultiplier;
		default:
			return row->Damage + BareHandDamage;
		}
	}

	////////////////////////////////////////////////
	//////////////// MOOD             //////////////
	////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Arousal;


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



	TObjectPtr<UStatus> Widget=nullptr;

	void setWidget(TObjectPtr<UStatus> widget) {
		Widget = widget;
		Widget->healthComponenet = this;
		resetWidget();
	}

	void resetWidget() {
		if (IsValid(Widget)) {
			if (Widget->Stamina)Widget->Stamina->SetPercent(Stamina / MaxStamina);
			if (Widget->Health)Widget->Health->SetPercent(getHealthPercentage());
			if (Widget->Magic)Widget->Magic->SetPercent(Magic / MaxMagic);
			if (Widget->Arousal)Widget->Arousal->SetPercent(Arousal);
		}
	}


	void tick( AGameCharacter * character, float DeltaTime);
};
