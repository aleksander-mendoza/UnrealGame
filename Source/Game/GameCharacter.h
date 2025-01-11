// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Logging/LogMacros.h"
#include "ui/TargetLockWidgetActor.h"
#include "ui/Inventory.h"
#include "items/ActorInventory.h" 
#include "items/Container.h" 
#include "character/Health.h" 
#include "anim/CharacterAnimInstance.h"
#include "GameCharacter.generated.h"

class USpringArmComponent;

struct FInputActionValue;
class AWorldGen;
struct Ray {
	FVector start, end;
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);



UCLASS(config=Game)
class AGameCharacter : public ACharacter , public ContainerEvents
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	/** First Person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	/** First Person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandle;
	double physicshandleDistance=-1;

	/** The female skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMesh> FemaleMesh;

	/** The male skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMesh> MaleMesh;

	/* The AnimBlueprint class to use. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<UCharacterAnimInstance> FemaleAnimClass;

	/* The AnimBlueprint class to use. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<UCharacterAnimInstance> MaleAnimClass;

	/** Is the character male or female */
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsFemale=true;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SlowWalkSpeed = 100;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 300;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 500;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UClass * WidgetClass;
	AActor* TargetLockActor = nullptr;
	/** Max interaction distance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float InteractDistance = 300;
	/** Max distance for locking onto a target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float TargetLockDistance = 1000;
	/** How quickly camera zooms in/out when user scrolls*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed = 20.0;
	/** Max camera zooms in out*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MaxZoomOut = 400.;
	/** Min camera zooms out*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MinZoomOut = 80.0;
	
	unsigned int comboAnimIdx = 0;
	/** Single-handed (left-handed) attack combo Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> LeftHandedAttackComboAnims;

	/** Single-handed (right-handed) attack combo Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> RightHandedAttackComboAnims;

	/** Double-handed Attack Combo Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> DoubleHandedAttackComboAnims;

	/** Unarmed Attack Combo Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> BareRightHandAttackComboAnims;
	
	/** Unarmed Attack Combo Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> BareLeftHandAttackComboAnims;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> LeftHandedStealthAttackComboAnims;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> RightHandedStealthAttackComboAnims;

	bool IsAttackingLeftHanded = false;
	bool IsAttackingRightHanded = false;
	float AttackCooldown = 0;
	bool CanMove = true;
	/** Hand socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FName HandSocketR;
	/** Hand socket (right)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FName HandSocketL;

	UPROPERTY()
	TArray<TObjectPtr<USkeletalMeshComponent>> Clothes;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> LeftHandMesh;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> RightHandMesh;

	TObjectPtr<UAnimMontage> CurrentAttackComboAnim = nullptr;
	bool CurrentAttackComboAnimIsLeftHanded = false;
public:

	
	const float ATTACK_COOLDOWN = 0.2;
	const float ATTACK_DISABLED = INFINITY;
	bool bIsSlowWalking = false;
	bool bIsRunning = false;
	bool bIsSwimming = false;
	AWorldGen* worldGenRef;
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Player Inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr <UActorInventory> Inventory;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr <UHealth> Health;

	AGameCharacter();
	

	inline bool isEnabled() const {
		return this->IsHidden();
	}
	inline void SetEnabled(bool enabled) {
		this->SetActorHiddenInGame(!enabled);
		this->SetActorEnableCollision(enabled);
	}
	/** Called for attack input */
	void LeftHandedAttackStart(const FInputActionValue& Value) {
		leftHandedAttackStart();
	}
	/** Called for attack input */
	void RightHandedAttackStart(const FInputActionValue& Value) {
		rightHandedAttackStart();
	}
	void leftHandedAttackStart() {
		IsAttackingLeftHanded = true;
		attackStart(true, LeftHandedStealthAttackComboAnims, LeftHandedAttackComboAnims, BareLeftHandAttackComboAnims);
	}
	void rightHandedAttackStart() {
		IsAttackingRightHanded = true;
		attackStart(false, RightHandedStealthAttackComboAnims, RightHandedAttackComboAnims, BareRightHandAttackComboAnims);
	}
	void attackStart(bool leftHand, TArray<TObjectPtr<UAnimMontage>> & stealthAttackComboAnims, TArray<TObjectPtr<UAnimMontage>>&  attackComboAnims, TArray<TObjectPtr<UAnimMontage>>& bare) {
		
		if (AttackCooldown<=0) {
			check(CurrentAttackComboAnim == nullptr);
			CurrentAttackComboAnim = nullptr;
			switch (Inventory->getWeaponClass(leftHand)) {
			case EItemClass::BOW: 
				if (leftHand) {
					UCharacterAnimInstance* i = getAnimInstance();
					i->AimsBow = true;
					i->ShotBow = false;
					AttackCooldown = ATTACK_DISABLED;
					break;
				}// TODO: right handed attack with bow allows for zoom-in
			case EItemClass::DOUBLE_HANDED_WEAPON:
				if (leftHand && DoubleHandedAttackComboAnims.Num() > 0) {
					CurrentAttackComboAnim = DoubleHandedAttackComboAnims[comboAnimIdx++ % DoubleHandedAttackComboAnims.Num()];
				}// TODO: right handed attack with double handed weapon works like blocking
				break;
			case EItemClass::SINGLE_HANDED_QUIET_WEAPON:
				if (stealthAttackComboAnims.Num() > 0) {
					CurrentAttackComboAnim = stealthAttackComboAnims[comboAnimIdx++ % stealthAttackComboAnims.Num()];
				}
				break;
			case EItemClass::SINGLE_HANDED_WEAPON:
				if (attackComboAnims.Num() > 0) {
					CurrentAttackComboAnim = attackComboAnims[comboAnimIdx++ % attackComboAnims.Num()];
				}
				break;
			default:
				if (bare.Num() > 0) {
					CurrentAttackComboAnim = bare[comboAnimIdx++ % bare.Num()];
				}
			}
			if (CurrentAttackComboAnim != nullptr) {
				AttackCooldown = ATTACK_DISABLED;
				CurrentAttackComboAnimIsLeftHanded = leftHand;
				getAnimInstance()->Montage_Play(CurrentAttackComboAnim);
			}
		}
	}
	
	/** Called for attack end input */
	void LeftHandedAttackEnd(const FInputActionValue& Value) {
		UCharacterAnimInstance * i = getAnimInstance(); 
		if (i->AimsBow) {
			i->AimsBow = false;
			i->ShotBow = true;
			AttackCooldown = ATTACK_COOLDOWN;
		}
		IsAttackingLeftHanded = false;
	}
	void RightHandedAttackEnd(const FInputActionValue& Value) {
		IsAttackingRightHanded = false;
	}
	/** Called for attack cancel input */
	void AttackCancel(const FInputActionValue& Value) {
		attackCancel();
	}
	void attackCancel() {
		UCharacterAnimInstance* i = getAnimInstance();
		if (CurrentAttackComboAnim == nullptr) {
			if (i->AimsBow) {
				i->AimsBow = false;
				AttackCooldown = ATTACK_COOLDOWN;
			}
		}else{
			check(i->AimsBow == false);
			i->Montage_Stop(ATTACK_COOLDOWN, CurrentAttackComboAnim);
			CurrentAttackComboAnim = nullptr;
			AttackCooldown = ATTACK_COOLDOWN;
		}
		
		IsAttackingLeftHanded = false;
		IsAttackingRightHanded = false;
	}
	/** Called for lock-on input */
	void LockOntoEnemy(const FInputActionValue& Value);
	/** Called for interaction input */
	void InteractStart(const FInputActionValue& Value);
	/** Called for interaction input */
	void InteractEnd(const FInputActionValue& Value);
	/** Called for camera zoom input */
	void CameraZoomIn(const FInputActionValue& Value);
	void CameraZoomOut(const FInputActionValue& Value);
	void CameraZoom(const FInputActionValue& Value);
	/** Called for movement input */
	void Move(const FInputActionValue& Value);


	void ToggleCrouch(const FInputActionValue& Value) {
		if (this->bIsCrouched) {
			this->uncrouch(Value);
		}
		else {
			this->crouch(Value);
		}
	}
	void StartSlowWalk(const FInputActionValue& Value)
	{
		if (!bIsSwimming && !bIsCrouched) {
			bIsRunning = false;
			GetCharacterMovement()->MaxWalkSpeed = SlowWalkSpeed;
			getAnimInstance()->IsSlowWalking = bIsSlowWalking = true;
		}
	}
	void endSlowWalk()
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		getAnimInstance()->IsSlowWalking = bIsSlowWalking = false;
	}
	void EndSlowWalk(const FInputActionValue& Value)
	{
		endSlowWalk();
	}
	void ToggleSlowWalk(const FInputActionValue& Value)
	{
		if (bIsSlowWalking) {
			EndSlowWalk(Value);
		}
		else {
			StartSlowWalk(Value);
		}
	}
	void StartRun(const FInputActionValue& Value)
	{
		if (!bIsSwimming && Health->Stamina > 10) {
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			bIsRunning = true;
			this->UnCrouch();
			getAnimInstance()->IsSlowWalking = bIsSlowWalking = false;
		}
	}
	void EndRun(const FInputActionValue& Value) {
		endRun();
	}
	void endRun()
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	void uncrouch(const FInputActionValue& Value)
	{
		this->UnCrouch();
	}
	void crouch(const FInputActionValue& Value)
	{
		if (!bIsSwimming) {
			if (bIsRunning) {
				EndRun(Value);
			}
			else if (bIsSlowWalking) {
				EndSlowWalk(Value);
			}
			this->Crouch();
		}
		
	}
	void SetJiggleStiffness(float jiggleStiffness) {
		getAnimInstance()->JiggleStiffness = jiggleStiffness;
	}
	void SetJiggleDamping(float jiggleDamping) {
		getAnimInstance()->JiggleDamping = jiggleDamping;
	}
	void SetJiggleStrength(float jiggleStrength) {
		getAnimInstance()->JiggleStrength= jiggleStrength;
	}
	/**If actor is null pinter then it clears the lock*/
	void LockOntoTarget(AActor * target);
	/**Toggle between first person camera and third person camera.*/
	void ToggleCamera(bool firstPersonView);
	/**Sets whether user can move in any direction. If false, user will move in camera direction. If true, movement will be independent from camera direction*/
	void ToggleDirectionalMovement(bool trueDirectionalMovement);
	/**if distance is distance<MinZoomOut then switches to first person view. Otherwise switches to third person and sets camera boom length to desired distance.
	The distance is clamped to MaxZoomOut. */
	void SetCameraDistance(float distance);

	UCameraComponent* GetCurrentCamera() {
		return FollowCamera->IsActive() ? FollowCamera : FirstPersonCamera;
	}

	void OnComboPartEnd(bool isLast) {
		if (isLast) {
			AttackCooldown = ATTACK_COOLDOWN;
			CurrentAttackComboAnim = nullptr;
		}
		else if (!(CurrentAttackComboAnimIsLeftHanded?IsAttackingLeftHanded:IsAttackingRightHanded)) {
			attackCancel();
		}
	}

	void SetSwimming(bool isSwimming) {
		if (isSwimming) {
			if (bIsSlowWalking)endSlowWalk();
			if (bIsRunning)endRun();
		}
		getAnimInstance()->IsSwimming = this->bIsSwimming = isSwimming;
	}

	void getRay(double length, Ray& ray);
	FVector getRayEnd(double length);

	

	virtual void OnEquipBothHands(TObjectPtr < UItemObject > item) override {
		RightHandMesh->SetStaticMesh(item->getMesh());
	}
	virtual void OnEquipLeftHand(TObjectPtr < UItemObject > item) override {
		LeftHandMesh->SetStaticMesh(item->getMesh());
	}
	virtual void OnEquipRightHand(TObjectPtr < UItemObject > item) override {
		RightHandMesh->SetStaticMesh(item->getMesh());
	}
	virtual void OnUnequipBothHands(TObjectPtr < UItemObject > item) override {
		LeftHandMesh->SetStaticMesh(nullptr);
	}
	virtual void OnUnequipLeftHand(TObjectPtr < UItemObject > item) override {
		LeftHandMesh->SetStaticMesh(nullptr);
	}
	virtual void OnUnequipRightHand(TObjectPtr < UItemObject > item) override {
		RightHandMesh->SetStaticMesh(nullptr);
	}
	virtual void OnEquipClothes(TObjectPtr < UItemObject > item) override;
	virtual void OnUnequipClothes(TObjectPtr < UItemObject > item) override {
		const int idx = item->equippedAt;
		Clothes[idx]->UnregisterComponent();
		Clothes.RemoveAtSwap(idx);
	}
	virtual void OnPutItem(TObjectPtr<UItemObject> item) override {}
	virtual void OnDropItem(TObjectPtr<UItemObject> item) override;
	virtual bool CanWear(const FItem& item) override {
		if (IsFemale)return item.Class == EItemClass::FEMALE_CLOTHES;
		else return item.Class == EItemClass::MALE_CLOTHES;
	}
	UCharacterAnimInstance* animInstane=nullptr;
	UCharacterAnimInstance* getAnimInstance() {
		if (animInstane == nullptr)animInstane = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
		check(animInstane);
		return animInstane;
	}
	inline void SetGender(const  bool isFemale) {
		if (isFemale != IsFemale) {
			IsFemale = isFemale;
			USkeletalMeshComponent * mesh = GetMesh();
			mesh->SetSkeletalMesh(isFemale ? FemaleMesh : MaleMesh);
			mesh->SetAnimInstanceClass(isFemale ? FemaleAnimClass : MaleAnimClass);
			animInstane = nullptr;
				
		}
	}
	
private:

	


	
protected:
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
};

