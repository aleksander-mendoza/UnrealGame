// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../items/item/Item.h"
#include "../items/inv/GameCharacterInventory.h"
#include "MovementSpeed.h"
#include "../anim/MeleeAttackClass.h"
#include "../anim/AttackHandedness.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UGameCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	UGameCharacterMovementComponent();

	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	USkeletalMeshComponent* GetMesh() const;

public:
	
	

	/** Player Inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr <UGameCharacterInventory> Inventory;

	
	
	EMovementSpeed MovementSpeed = EMovementSpeed::WALK;

	class AGameCharacter * GameCharacter;
	


	void ToggleSlowWalking()
	{
		if (IsSlowWalking()) {
			StartWalking();
		}
		else {
			StartSlowWalking();
		}
	}
	inline bool AimsBow() {
		return Inventory->AimsBow();
	}
	inline bool CanJump() {
		return Inventory->CanJump();
	}
	inline void StartWalking() {
		MovementSpeed = EMovementSpeed::WALK;
		MaxWalkSpeed = Inventory->Health.WalkSpeed;
	}
	inline bool CanStartRunning() {
		return Inventory->CanRun();
	}
	inline void StartRunning() {
		if (CanStartRunning()) {
			MovementSpeed = EMovementSpeed::RUN;
			MaxWalkSpeed = Inventory->Health.RunSpeed;
		}
	}
	inline void StartSlowWalking() {
		MovementSpeed = EMovementSpeed::SLOW_WALK;
		MaxWalkSpeed = Inventory->Health.SlowWalkSpeed;
	}
	inline EArmedPoseType getArmedStatus() {
		return Inventory->getArmedStatus();
	}
	inline bool IsSheathed() {
		return Inventory->IsSheathed();
	}
	inline bool IsBowShot() {
		return Inventory->IsBowShot();
	}
	inline bool IsRunning() {
		return IsWalking() && MovementSpeed == EMovementSpeed::RUN;
	}
	inline bool IsSlowWalking() {
		return IsWalking() && MovementSpeed == EMovementSpeed::SLOW_WALK;
	}

	inline bool IsDirectionalMovement() {
		return bOrientRotationToMovement;
	}
	inline void ToggleDirectionalMovement(bool trueDirectionalMovement) {
		bOrientRotationToMovement = trueDirectionalMovement;
		bUseControllerDesiredRotation = !trueDirectionalMovement;
	}
	inline void Move(const FVector & MovementVector, const FRotator & Rotation) {
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if (IsSwimming()) {
			// add movement 
			const FVector UpDirection(0, 0, 1);
			AddInputVector(UpDirection * MovementVector.Z);
		}
		AddInputVector(ForwardDirection * MovementVector.Y);
		AddInputVector(RightDirection * MovementVector.X);
	}
	
	

	
	inline bool CanAttack() {
		return !IsSwimming() && !IsFalling() && Inventory->CanAttack();
	}

	inline void startAttack(bool leftHand) {
		if (CanAttack()) {
			Inventory->startAttack(leftHand);
		}
	}
	void endAttack(bool leftHand) {
		Inventory->endAttack(leftHand);
	}
	
	void cancelAttack() {
		Inventory->cancelAttack();
		
	}
	
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
