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


	UPROPERTY()
	TArray<TObjectPtr<USkeletalMeshComponent>> Clothes;

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
	void Interact(const FInputActionValue& Value);
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

	inline void SetGender(const  bool isFemale) {
		if (isFemale != IsFemale) {
			IsFemale = isFemale;
			USkeletalMeshComponent * mesh = GetMesh();
			mesh->SetSkeletalMesh(isFemale ? FemaleMesh : MaleMesh);
			mesh->SetAnimInstanceClass(isFemale ? FemaleAnimClass : MaleAnimClass);
		}
	}
	/**returns true if the clothing item is wearable and matches the gender/race of this character*/
	inline bool canWear(const FItem & item) const {
		if (IsFemale)return item.Class == EItemClass::FEMALE_CLOTHES;
		else return item.Class == EItemClass::MALE_CLOTHES;
	}
	
	
	/**Returns false if the item couldn't be equipped. Potential reasons would be
	1. it wasn't a clothing item 
	2. it was clothing item but not for this character (mismatched geneder/race)
	3. unequipClothesWithOverlappingSlots was set to false and the character was already wearing clothes with overlapping slots
	4. unequipClothesWithOverlappingSlots was set to true but one of the already equipped conflicting items was devious */
	bool putOnClothingItem(UItemObject* item, bool unequipClothesWithOverlappingSlots=true);
	/**Returns false if the item couldn't be unequipped. This can happend in case of clothes that are marked as devious. Use force=true to force the lock open*/
	bool takeOffClothingItem(UItemObject* item, bool force = false);
	/**Returns false if the item couldn't be equipped (and the old item also was not uneqipped). Potential reasons would be
	1. it wasn't a clothing item
	2. it was clothing item but not for this character (mismatched geneder/race)
	3. unequipClothesWithOverlappingSlots was set to false and the character was already wearing clothes with overlapping slots.
	4. the old item was devious
	5. unequipClothesWithOverlappingSlots was set to true but one of the already equipped conflicting items was devious
	*/
	//bool swapClothingItem(UItemObject* oldItem, UItemObject* newItem, bool unequipClothesWithOverlappingSlots = true);
private:
	inline void addClothingComponent(UItemObject* item) {
		const FItem* row = item->Instance.getRow();
		if (!row->WearableMesh.IsValid()) {
			row->WearableMesh.LoadSynchronous();
		}
		USkeletalMesh* clothingMesh = row->WearableMesh.Get();
		USkeletalMeshComponent* clothingComp = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass());
		USkeletalMeshComponent* playerMesh = GetMesh();
		clothingComp->RegisterComponent();
		clothingComp->AttachToComponent(playerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
		clothingComp->SetSkeletalMesh(clothingMesh);
		clothingComp->SetAnimInstanceClass(playerMesh->AnimClass);
		clothingComp->SetLeaderPoseComponent(playerMesh, true);
		const int idx = Clothes.Add(clothingComp);
		checkf(item->equippedAt == idx, TEXT("%d != %d"), item->equippedAt, idx);
		check(Inventory->Clothes[item->equippedAt] == item);
	}

	inline void resetClothes(){
		for (int i = 0; i < Inventory->Clothes.Num(); i++) {
			addClothingComponent(Inventory->Clothes[i]);
		}
	}

	template<bool onlyIfNotConflicting>
	inline bool addClothingItem(UItemObject* item) {
		if (Inventory->addClothingItem<onlyIfNotConflicting>(item)) {
			addClothingComponent(item);
			return true;
		}
		else {
			return false;
		}
	}
	template<bool onlyIfNotDevious>
	inline bool removeClothingItem(UItemObject* item) {
		const int idx = item->equippedAt;
		if (Inventory->removeClothingItem<onlyIfNotDevious>(item)) {
			Clothes[idx]->UnregisterComponent();
			Clothes.RemoveAtSwap(idx);
			check(idx < Inventory->Clothes.Num() || idx == Inventory->Clothes[idx]->equippedAt);
			return true;
		}
		else {
			return false;
		}
	}
	template<bool onlyIfNotDevious>
	inline bool makeSpaceFor(const FItem& item) {
		for (int clothesIdx = 0; clothesIdx < Inventory->Clothes.Num(); clothesIdx++) {
			UItemObject* equippedItemObj = Inventory->Clothes[clothesIdx];
			const FItem* equippedItem = equippedItemObj->Instance.getRow();
			if (equippedItem->conflictsWith(item)) {
				if (!removeClothingItem<onlyIfNotDevious>(equippedItemObj)) {
					return false;
				}
			}
		}
		return true;
	}
protected:
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
};

