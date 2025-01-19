// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacterMovementComponent.h"
#include "../GameCharacter.h"

UGameCharacterMovementComponent::UGameCharacterMovementComponent() {
	JumpZVelocity = 700.f;
	AirControl = 0.35f;
	MaxWalkSpeed = 500.f;
	MinAnalogWalkSpeed = 20.f;
	BrakingDecelerationWalking = 2000.f;
	BrakingDecelerationFalling = 1500.0f;
	GetNavAgentPropertiesRef().bCanCrouch = true;
	GetNavAgentPropertiesRef().bCanSwim = true;
}

void UGameCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGameCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	WalkSpeed = MaxWalkSpeed;
}

UAnimInstance* UGameCharacterMovementComponent::getAnimInstance() const
{
	return GameCharacter->getAnimInstance();
}
