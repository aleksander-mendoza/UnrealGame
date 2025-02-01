// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat.h"
#include "../GameCharacter.h"

FCombat::FCombat() {
	HandSocketR = TEXT("hand_rSocket");
	HandSocketL = TEXT("hand_lSocket");
	SheathedSocketL = TEXT("sheathed_lSocket");
	SheathedSocketR = TEXT("sheathed_rSocket");
	SheathedSocketBackL = TEXT("sheathed_back_lSocket");
	SheathedSocketBackR = TEXT("sheathed_back_rSocket");
}

