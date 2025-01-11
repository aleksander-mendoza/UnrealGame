// Fill out your copyright notice in the Description page of Project Settings.


#include "Status.h"
#include "../character/Health.h"

void UStatus::NativeConstruct()
{
	if (healthComponenet != nullptr) {
		healthComponenet->resetWidget();
	}
}
