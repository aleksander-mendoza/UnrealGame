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


	Combat.LeftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHandMesh"));
	Combat.RightHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHandMesh"));
	Combat.LeftHandMesh->SetSimulatePhysics(false);
	Combat.RightHandMesh->SetSimulatePhysics(false);
	Combat.LeftHandMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Combat.RightHandMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Combat.RightHandMesh->RegisterComponent();
	Combat.LeftHandMesh->RegisterComponent();
}

void UGameCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGameCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	WalkSpeed = MaxWalkSpeed;
	USkeletalMeshComponent* Mesh = GetMesh();
	Combat.BareHandSocket[true] = Mesh->GetSocketByName(Combat.HandSocketL);
	Combat.BareHandSocket[false] = Mesh->GetSocketByName(Combat.HandSocketR);
}

USkeletalMeshComponent* UGameCharacterMovementComponent::GetMesh() const
{
	return GameCharacter->GetMesh();
}

UAnimInstance* UGameCharacterMovementComponent::getAnimInstance()
{
	return GameCharacter->getAnimInstance();
}

void UGameCharacterMovementComponent::HitDetectHand(bool leftHand)
{
	check(!GameCharacter->Inventory->IsDoubleHanded() || Combat.GetBladeStart(true) == nullptr);
	TArray<FHitResult> OutHit;
	if (Combat.HitDetectHand(leftHand, GetOwner(), GetWorld(), GetMesh(), OutHit)) {
		UItemObject* item = GameCharacter->Inventory->getHandItem(leftHand);
		const float damage = Health.getDamage(item);
		for (int i = 0; i < OutHit.Num(); i++) {
			IHittable* c = Cast< IHittable>(OutHit[i].GetActor());
			if (c)c->OnHit(GameCharacter, item, damage);
		}
	}
}

void UGameCharacterMovementComponent::Kill(AGameCharacter* actor)
{
	GameCharacter->Kill(actor);
}

void UGameCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (invincibilityDuration > 0) {
		invincibilityDuration -= DeltaTime;
	}
	Health.tick(DeltaTime);
	if (IsRunning()) {
		if (Health.UpdateRunning(DeltaTime)) {
			StartWalking();
		}

	}
	if (isPlayingAttackAnim()) {
		if (Combat.hitDetectionTimer > 0) {
			Combat.hitDetectionTimer -= DeltaTime;
		}
		else {
			HitDetect();
			Combat.hitDetectionTimer = HIT_DETECTION_PERIOD;
		}
	}
	else {
		if (TickCooldown(DeltaTime)) {
			attackStart(WantsToAttackLeft(), GameCharacter->Inventory);
		}
	}
}

