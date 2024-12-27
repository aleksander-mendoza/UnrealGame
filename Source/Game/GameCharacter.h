// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ui/TargetLockWidgetActor.h"
#include "ui/Inventory.h"
#include "items/ActorInventory.h" 
#include "GameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);



UCLASS(config=Game)
class AGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** First Person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	/** Player Inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UActorInventory * Inventory;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Lock Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LockAction;

	/** Zoom In Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ZoomInAction;

	/** Zoom Out Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ZoomOutAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Lock Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* OpenInventoryAction;

	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UInventory> InventoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UClass * WidgetClass;
	AActor* TargetLockActor = nullptr;

	/** How quickly camera zooms in/out when user scrolls*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed = 20.0;
	float MaxZoomOut = 400.;
	float MinZoomOut = 120.0;
	
	/** Attack Combo Animation*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackComboAnim;
	bool IsAttacking=false;
	bool DoNextCombo = false;

	UPROPERTY()
	TObjectPtr<UInventory> InventoryInterface;

	UPROPERTY()
	TArray<USkeletalMeshComponent*> Clothes;
public:
	AGameCharacter();
	
	void ResetClothes();

protected:
	/** Called for attack input */
	void Attack(const FInputActionValue& Value);
	/** Called for lock-on input */
	void LockOntoEnemy(const FInputActionValue& Value);
	/** Called for interaction input */
	void Interact(const FInputActionValue& Value);
	/** Called for camera zoom in input */
	void CameraZoomIn(const FInputActionValue& Value);
	/** Called for camera zoom out input */
	void CameraZoomOut(const FInputActionValue& Value);

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called on open/close inventory input */
	void TriggerInventory(const FInputActionValue& Value);
public:
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

	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

