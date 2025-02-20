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


	Inventory = CreateDefaultSubobject<UGameCharacterInventory>(TEXT("PlayerInventory"));
}

void UGameCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGameCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	MaxWalkSpeed = Inventory->Health.GetWalkSpeed();
}

USkeletalMeshComponent* UGameCharacterMovementComponent::GetMesh() const
{
	return GameCharacter->GetMesh();
}

void UGameCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!Inventory->IsDead()) {
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		if (IsRunning()) {
			Inventory->Health.Stamina -= 10. * DeltaTime;
			if (Inventory->Health.Stamina < 0) {
				Inventory->Health.PreventStaminaRegen = 1;
				StartWalking();
			}
		}
		Inventory->TickEverything(DeltaTime);
	}
}

