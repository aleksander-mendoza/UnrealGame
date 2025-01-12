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
		if (IsSwimming) {
			GroundSpeed = Velocity.Length();
		}
		else {
			GroundSpeed = double2(Velocity).Length();
		}
		const double3 acc = MovementComponent->GetCurrentAcceleration();
		ShouldMove = acc != double3(0, 0, 0) && GroundSpeed > 3.;
		IsFalling = MovementComponent->IsFalling();
		const FRotator rot = Character->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, rot);
		IsCrouching = MovementComponent->IsCrouching();
	}
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	if (!IsValid(Character)) {
		Character = Cast<AGameCharacter>(GetOwningActor());
		if(Character!=nullptr) MovementComponent = Character->GetCharacterMovement();
	}
}
