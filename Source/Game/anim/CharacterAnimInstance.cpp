// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAnimInstance.h"
#include "../GameCharacter.h"
#include "../blender/utildefines.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"


void UCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	if (IsValid(MovementComponent)) {
		Velocity = MovementComponent->Velocity;
		if (MovementComponent->IsSwimming()) {
			GroundSpeed = Velocity.Length();
		}
		else {
			GroundSpeed = double2(Velocity).Length();
		}
		const double3 acc = MovementComponent->GetCurrentAcceleration();
		ShouldMove = acc != double3(0, 0, 0) && GroundSpeed > 3.;
		const FRotator rot = Character->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, rot);
		ArmedPoseType = MovementComponent->getArmedStatus();
		IsBowShot = MovementComponent->IsBowShot();
		IsCrouching = MovementComponent->IsCrouching();
		IsSwimming = MovementComponent->IsSwimming();
		IsRunning = MovementComponent->IsRunning();
		IsWalking = MovementComponent->IsWalking();
		IsSlowWalking = MovementComponent->IsSlowWalking();
		IsFalling = MovementComponent->IsFalling();
		AimsBow = MovementComponent->AimsBow();
		IsStrafe = MovementComponent->IsDirectionalMovement();
		IsSheathed = MovementComponent->Combat.isSheathed();
	}
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	if (!IsValid(Character)) {
		Character = Cast<AGameCharacter>(GetOwningActor());
		if(Character!=nullptr) MovementComponent = Character->GameMovement;
	}
}
