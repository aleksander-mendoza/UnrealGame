// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Logging/LogMacros.h"
#include "ui/TargetLockWidgetActor.h"
#include "ui/Inventory.h"
#include "proc_assets/WorldGen.h"
#include "items/ActorInventory.h" 
#include "items/Container.h" 
#include "GameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
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
	class TSubclassOf<UAnimInstance> FemaleAnimClass;

	/* The AnimBlueprint class to use. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<UAnimInstance> MaleAnimClass;

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
	
	/** Attack Combo Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackComboAnim;
	bool IsAttacking=false;
	bool DoNextCombo = false;
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

	AWorldGen* worldGenRef;

public:
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Player Inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr <UActorInventory> Inventory;

	AGameCharacter();
	

	/** Called for attack input */
	void Attack(const FInputActionValue& Value);
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


	void ToggleSlowWalk(const FInputActionValue& Value);
	void StartRun(const FInputActionValue& Value);
	void EndRun(const FInputActionValue& Value);


	/**If actor is null pinter then it clears the lock*/
	void LockOntoTarget(AActor * target);
	/**Toggle between first person camera and third person camera.*/
	void ToggleCamera(bool firstPersonView);
	/**Sets whether user can move in any direction. If false, user will move in camera direction. If true, movement will be independent from camera direction*/
	void ToggleDirectionalMovement(bool trueDirectionalMovement);
	/**if distance is distance<MinZoomOut then switches to first person view. Otherwise switches to third person and sets camera boom length to desired distance.
	The distance is clamped to MaxZoomOut. */
	void SetCameraDistance(float distance);

	UCameraComponent* GetCurrentCamera();

	void OnComboPartEnd(bool isLast);

	void SetSwimming(bool isSwimming);

	void getRay(double length, Ray& ray);
	FVector getRayEnd(double length);

	

	virtual void OnEquipBothHands(TObjectPtr < UItemObject > item) override {
		LeftHandMesh->SetStaticMesh(item->getMesh());
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

	inline void SetGender(const  bool isFemale) {
		if (isFemale != IsFemale) {
			IsFemale = isFemale;
			USkeletalMeshComponent * mesh = GetMesh();
			mesh->SetSkeletalMesh(isFemale ? FemaleMesh : MaleMesh);
			mesh->SetAnimInstanceClass(isFemale ? FemaleAnimClass : MaleAnimClass);
		}
	}
	
private:

	


	
protected:
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
};

