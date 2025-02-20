// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Logging/LogMacros.h"
#include "ui/TargetLockWidgetActor.h"
#include "anim/AttackHandedness.h"
#include "items/Container.h" 
#include "character/Hittable.h" 
#include "character/Interactable.h"
#include "character/Hairdo.h"
#include "ui/NpcHealthBar.h"

#include "character/GameCharacterMovementComponent.h"
#include "anim/CharacterAnimInstance.h"
#include "GameCharacter.generated.h"

class USpringArmComponent;

struct FInputActionValue;
namespace DialogueDatabase {
	class DialogueStage;
}
struct Ray {
	FVector start, end;
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);



UCLASS(config=Game)
class AGameCharacter : public ACharacter, public IHittable, public IInteractable
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	FText CharacterName;

	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	FLinearColor HairColor = FLinearColor(0., 0., 1.);


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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true", RowType = "Hairdo"))
	FDataTableRowHandle Hairdo;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> HairMeshComponent;



public:
	class AGamePlayerController* GameController = nullptr;


	FDataTableRowHandle getHairdo() const {
		return Hairdo;
	}
	bool IsPlayer() const{
		return GameController != nullptr;
	}

	inline const FText & GetCharacterName() const {
		return CharacterName;
	}

	UGameCharacterMovementComponent* GameMovement=nullptr;
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }



	UCharacterAnimInstance* animInstane = nullptr;
	UCharacterAnimInstance* getAnimInstance() {
		if (animInstane == nullptr)animInstane = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
		check(GetMesh()->GetAnimInstance());
		check(animInstane);
		return animInstane;
	}


	explicit AGameCharacter(const FObjectInitializer & ObjectInitializer);
	
	inline FVector getHeadLocation() {
		return GetPawnViewLocation(); //FirstPersonCamera->GetComponentLocation();
	}
	inline bool isEnabled() const {
		return this->IsHidden();
	}
	inline void SetEnabled(bool enabled) {
		this->SetActorHiddenInGame(!enabled);
		this->SetActorEnableCollision(enabled);
	}
	void attackCancel() {
		GameMovement->cancelAttack();
	}
	const DialogueDatabase::DialogueStage* dialogueStage;
	const DialogueDatabase::DialogueStage* getDialogueStage() {
		return dialogueStage;
	}
	bool OnInteract(class AGameCharacter* actor) override;
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
		GameMovement->startAttack(true);
	}
	/** Called for attack input */
	void RightHandedAttackStart(const FInputActionValue& Value) {
		GameMovement->startAttack(false);
	}
	/** Called for attack cancel input */
	void AttackCancel(const FInputActionValue& Value) {
		attackCancel();
	}
	void RightHandedAttackEnd(const FInputActionValue& Value) {
		GameMovement->endAttack(false);
	}
	/** Called for attack end input */
	void LeftHandedAttackEnd(const FInputActionValue& Value) {
		GameMovement->endAttack(true);
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
	class AItemActorProjectile * Shoot(double speed);
	void NotifyAttackAnimationFinished() {
		GameMovement->Inventory->NotifyAttackAnimationFinished();
	}
	void NotifyBowReadyToShoot() {
		GameMovement->Inventory->NotifyBowReadyToShoot();
	}
	void OnComboPartEnd() {
		GameMovement->Inventory->OnComboPartEnd();
	}

	void getRay(double length, Ray& ray);
	FVector getRayEnd(double length);


	inline void DisableWeaponTrace() {
		GameMovement->Inventory->DisableWeaponTrace();
	}
	
	inline void EnableWeaponTrace(bool enableLeftHandHitDetection, bool enableRightHandHitDetection, bool enableLeftFootHitDetection, bool enableRightFootHitDetection) {
		GameMovement->Inventory->EnableWeaponTrace(enableLeftHandHitDetection, enableRightHandHitDetection, enableLeftFootHitDetection, enableRightFootHitDetection);
	}
	
	void Kill() {//TODO: kill moves that involve the killer
		GameMovement->Inventory->kill();
	}
	
	virtual void OnHit(UInventoryAndHealth* actor, UItemInstance* weaponUsed, UItemInstance* projectile, float damage) override {
		GameMovement->Inventory->onHit(actor, weaponUsed, projectile, damage);
	}


	
	inline void SetGender(const  bool isFemale) {
		if (isFemale != GameMovement->Inventory->IsFemale()) {
			USkeletalMeshComponent * mesh = GetMesh();
			mesh->SetSkeletalMesh(isFemale ? FemaleMesh : MaleMesh);
			mesh->SetAnimInstanceClass(isFemale ? FemaleAnimClass : MaleAnimClass);
			GameMovement->Inventory->Health.IsFemale = isFemale;
			animInstane = nullptr;
				
		}
	}
	UMaterialInstanceDynamic* DynamicHairMaterial=nullptr;
	inline void setHairdo(FDataTableRowHandle hairdo) {
		Hairdo = hairdo;
		USkeletalMesh* mesh=nullptr;
		if (!Hairdo.IsNull()) {
			mesh = Hairdo.GetRow<FHairdo>("")->getSkeletalMesh();
			if (DynamicHairMaterial == nullptr) {
				FSkeletalMaterial& mat = mesh->GetMaterials()[0];
				DynamicHairMaterial = UMaterialInstanceDynamic::Create(mat.MaterialInterface, this);
				DynamicHairMaterial->SetVectorParameterValue("color", HairColor);
			}
			HairMeshComponent->SetMaterial(0, DynamicHairMaterial);
		}
		HairMeshComponent->SetSkeletalMesh(mesh);
	}
	inline FLinearColor getHairColor() {
		return HairColor;
	}
	void setHairColor(FLinearColor rgb) {
		HairColor = rgb;
		if (DynamicHairMaterial != nullptr) {
			DynamicHairMaterial->SetVectorParameterValue("color", rgb);
		}
	}
	
	


	
protected:
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
};

