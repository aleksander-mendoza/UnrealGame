// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Logging/LogMacros.h"
#include "ui/TargetLockWidgetActor.h"
#include "anim/AttackHandedness.h"
#include "items/ActorInventory.h" 
#include "items/Container.h" 
#include "character/Hittable.h" 
#include "character/Interactable.h"

#include "character/GameCharacterMovementComponent.h"
#include "anim/CharacterAnimInstance.h"
#include "anim/WeaponSetAnims.h"
#include "GameCharacter.generated.h"

class USpringArmComponent;

struct FInputActionValue;

struct Ray {
	FVector start, end;
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);



UCLASS(config=Game)
class AGameCharacter : public ACharacter , public ContainerEvents, public IHittable, public IInteractable
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
	

	UPROPERTY()
	TArray<TObjectPtr<USkeletalMeshComponent>> Clothes;

	


public:
	

	UGameCharacterMovementComponent* GameMovement=nullptr;
	
	class AOpenWorld* worldRef;
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Player Inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr <UActorInventory> Inventory;

	

	UCharacterAnimInstance* animInstane = nullptr;
	UCharacterAnimInstance* getAnimInstance() {
		if (animInstane == nullptr)animInstane = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
		check(animInstane);
		return animInstane;
	}


	explicit AGameCharacter(const FObjectInitializer & ObjectInitializer);
	

	inline bool isEnabled() const {
		return this->IsHidden();
	}
	inline void SetEnabled(bool enabled) {
		this->SetActorHiddenInGame(!enabled);
		this->SetActorEnableCollision(enabled);
	}
	inline void attackEnd(bool leftHanded) {
		GameMovement->attackEnd(leftHanded);
	}
	void attackStart(bool leftHanded) {
		GameMovement->attackStart(leftHanded, Inventory);
	}
	void attackCancel() {
		GameMovement->attackCancel();
	}
	virtual bool OnInteract(class AGameCharacter* actor)  {
		//TODO: start dialogue
		return true;
	}
	/** Called for lock-on input */
	void LockOntoEnemy(const FInputActionValue& Value);
	AActor* interactedActor;
	/** Called for interaction input */
	void InteractTriggered(const FInputActionValue& Value) {
		if (IInteractable* i = Cast<IInteractable>(interactedActor)) {
			i->OnInteract(this);
		}
	}
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

	/** Called for attack input */
	void LeftHandedAttackStart(const FInputActionValue& Value) {
		attackStart(true);
	}
	/** Called for attack input */
	void RightHandedAttackStart(const FInputActionValue& Value) {
		attackStart(false);
	}
	/** Called for attack cancel input */
	void AttackCancel(const FInputActionValue& Value) {
		attackCancel();
	}
	void RightHandedAttackEnd(const FInputActionValue& Value) {
		attackEnd(false);
	}
	/** Called for attack end input */
	void LeftHandedAttackEnd(const FInputActionValue& Value) {
		attackEnd(true);
	}
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
		GameMovement->StartSlowWalking();
	}
	void endSlowWalk()
	{
		GameMovement->StartWalking();
	}
	void EndSlowWalk(const FInputActionValue& Value)
	{
		GameMovement->StartWalking();
	}
	void ToggleSlowWalk(const FInputActionValue& Value)
	{
		GameMovement->ToggleSlowWalking();
	}
	void StartRun(const FInputActionValue& Value)
	{
		GameMovement->StartRunning();
	}
	void EndRun(const FInputActionValue& Value) {
		GameMovement->StartWalking();
	}
	void uncrouch(const FInputActionValue& Value)
	{
		this->UnCrouch();
	}
	void crouch(const FInputActionValue& Value)
	{
		this->Crouch();
	}

	void Jump() override
	{
		if (GameMovement->CanJump())Super::Jump();
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
	void ToggleCamera(bool firstPersonView) {
		FollowCamera->SetActive(!firstPersonView);
		FirstPersonCamera->SetActive(firstPersonView);
	}
	/**Sets whether user can move in any direction. If false, user will move in camera direction. If true, movement will be independent from camera direction*/
	void ToggleDirectionalMovement(bool trueDirectionalMovement) {
		GameMovement->ToggleDirectionalMovement(trueDirectionalMovement);
	}
	/**if distance is distance<MinZoomOut then switches to first person view. Otherwise switches to third person and sets camera boom length to desired distance.
	The distance is clamped to MaxZoomOut. */
	void SetCameraDistance(float distance);

	UCameraComponent* GetCurrentCamera() {
		return FollowCamera->IsActive() ? FollowCamera : FirstPersonCamera;
	}
	void NotifyAttackAnimationFinished() {
		GameMovement->NotifyAttackAnimationFinished();
	}
	void OnComboPartEnd() {
		GameMovement->OnComboPartEnd();
	}

	void getRay(double length, Ray& ray);
	FVector getRayEnd(double length);


	inline void DisableWeaponTrace() {
		GameMovement->Combat.DisableWeaponTrace();
	}
	
	inline void EnableWeaponTrace(bool enableLeftHandHitDetection, bool enableRightHandHitDetection, bool enableLeftFootHitDetection, bool enableRightFootHitDetection) {
		GameMovement->Combat.EnableWeaponTrace(enableLeftHandHitDetection, enableRightHandHitDetection, enableLeftFootHitDetection, enableRightFootHitDetection);
	}
	
	
	void Kill(AGameCharacter* killer) {//TODO: kill moves that involve the killer
		TObjectPtr<UAnimMontage> anim = GameMovement->Combat.getDeathAnim();
		if (anim) {
			getAnimInstance()->Montage_Play(anim);
		}
		GetMesh()->SetSimulatePhysics(true);
	}
	
	virtual void OnHit(AGameCharacter * actor, UItemObject* weaponUsed, float damage) override{
		GameMovement->Hit(actor, weaponUsed, damage);
	}
	virtual void OnEquipBothHands(TObjectPtr < UItemObject > item) override {
		GameMovement->Combat.EquipItemRight(item);
		GameMovement->Combat.sheathBackRight(GetMesh());
	}
	virtual void OnEquipLeftHand(TObjectPtr < UItemObject > item) override {
		OnEquip(true, item);
	}
	virtual void OnEquipRightHand(TObjectPtr < UItemObject > item) override {
		OnEquip(false, item);
	}
	inline void OnEquip(bool leftHand, TObjectPtr < UItemObject > item) {
		GameMovement->Combat.EquipItem(leftHand, item);
		bool wasUnsheathed = !GameMovement->Combat.isSheathed();
		GameMovement->Combat.sheath(false, item->isSheathedOnTheBack(), GetMesh());
		if (wasUnsheathed) {// this will happen if user changes weapon before unsheathing it
			GameMovement->becomeArmed(Inventory);
		}
	}
	virtual void OnUnequipBothHands(TObjectPtr < UItemObject > item) override {
		OnUnequipRightHand(item);
	}
	virtual void OnUnequipLeftHand(TObjectPtr < UItemObject > item) override {
		GameMovement->Combat.UnequipLeftHand();
		GameMovement->becomeUnarmed();
	}
	virtual void OnUnequipRightHand(TObjectPtr < UItemObject > item) override {
		GameMovement->Combat.UnequipRightHand();
		GameMovement->becomeUnarmed();
	}
	virtual void OnEquipClothes(TObjectPtr < UItemObject > item) override;
	virtual void OnUnequipClothes(TObjectPtr < UItemObject > item) override {
		const int idx = item->equippedAt;
		Clothes[idx]->UnregisterComponent();
		Clothes.RemoveAtSwap(idx);
		GameMovement->Health.Defence -= item->getItemArmor();
	}
	virtual void OnPutItem(TObjectPtr<UItemObject> item) override {
		GameMovement->Health.CarriedWeight += item->getItemWeight();
	}
	virtual void OnDropItem(TObjectPtr<UItemObject> item) override;
	virtual bool CanWear(const FItem& item) override {
		if (IsFemale)return isFemaleWearable(item.Class);
		else return isMaleWearable(item.Class);
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
	
	


	
protected:
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
};

