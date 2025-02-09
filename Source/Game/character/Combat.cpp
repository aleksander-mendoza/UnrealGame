// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat.h"
#include "../GameCharacter.h"

FCombat::FCombat() {
	Left.HandSocket = TEXT("hand_lSocket");
	Right.HandSocket = TEXT("hand_rSocket");
	Left.SheathedSocket = TEXT("sheathed_lSocket");
	Right.SheathedSocket = TEXT("sheathed_rSocket");
	Left.SheathedSocketBack = TEXT("sheathed_back_lSocket");
	Right.SheathedSocketBack = TEXT("sheathed_back_rSocket");
}

