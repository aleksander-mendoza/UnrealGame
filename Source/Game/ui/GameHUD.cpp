// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"

AGameHUD::AGameHUD(){
	
}

void AGameHUD::BeginPlay()
{
	if(StatusWidget)StatusWidget->AddToViewport(9998);
}

