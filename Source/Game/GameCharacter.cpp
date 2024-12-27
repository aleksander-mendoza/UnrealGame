// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "blender/utildefines.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"
#include "anim/NotifyCombo.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGameCharacter

AGameCharacter::AGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeTransform(FTransform(FVector(0., 0., 40.)));
	FollowCamera->SetActive(true);

	// Create a follow camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), "head"); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FirstPersonCamera->SetRelativeTransform(FTransform(FRotator(0, 90, -90), FVector(0, 10, 0)));
	FirstPersonCamera->bUsePawnControlRotation = true; // Camera rotates relative to head
	FirstPersonCamera->SetActive(false);


	Inventory = CreateDefaultSubobject<UActorInventory>(TEXT("PlayerInventory"));
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

}

void AGameCharacter::ResetClothes()
{
	USkeletalMeshComponent * playerMesh = GetMesh();
	for (int i = 0; i < Inventory->Clothes.Num(); i++) { 
		const UItemObject * clothingItem = Inventory->Clothes[i];
		const FItem * row = clothingItem->Instance.getRow();
		if (row->WearableMesh) {
			USkeletalMesh* clothingMesh = row->WearableMesh.Get();
			USkeletalMeshComponent* clothingComp = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass());
			clothingComp->RegisterComponent();
			clothingComp->AttachToComponent(playerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
			clothingComp->SetSkeletalMesh(clothingMesh);
			clothingComp->SetAnimInstanceClass(playerMesh->AnimClass);
			clothingComp->SetLeaderPoseComponent(playerMesh, true);
			Clothes.Add(clothingComp);
		}
	}
}

void AGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	if (AttackComboAnim != nullptr) {
		int n = AttackComboAnim->Notifies.Num();
		for (int i = 0; i < n; i++) { 
			if (UNotifyCombo* notify = Cast<UNotifyCombo>(AttackComboAnim->Notifies[i].Notify)) {
				notify->Owner = this;
				notify->IsLast = i+1==n;
			}
		}
	}
	ToggleDirectionalMovement(true);
	Inventory->ResetToDefault();
	ResetClothes();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGameCharacter::ToggleCamera(bool firstPersonView) {
	FollowCamera->SetActive(!firstPersonView);
	FirstPersonCamera->SetActive(firstPersonView);
}
void AGameCharacter::SetCameraDistance(float distance) {
	if (distance < MinZoomOut) {
		ToggleCamera(true);
	} else {
		ToggleCamera(false);
		this->GetCameraBoom()->TargetArmLength = math::min(MaxZoomOut, distance);
	}
}
void AGameCharacter::CameraZoomOut(const FInputActionValue& Value) {
	if (this->FirstPersonCamera->IsActive()) {
		ToggleCamera(false);
	}
	else { // in third person view
		this->GetCameraBoom()->TargetArmLength = math::min(this->GetCameraBoom()->TargetArmLength + ZoomSpeed, MaxZoomOut);
	}
}
void AGameCharacter::CameraZoom(const FInputActionValue& Value) {
	float zoom = Value.Get<FInputActionValue::Axis1D>();
	if (zoom > 0)CameraZoomIn(Value);
	else CameraZoomOut(Value);
}
void AGameCharacter::CameraZoomIn(const FInputActionValue& Value) {
	
	if (this->FollowCamera->IsActive()) { // in third person view
		float arm = this->GetCameraBoom()->TargetArmLength;
		if (arm <= MinZoomOut) {
			ToggleCamera(true);
		}
		else {
			this->GetCameraBoom()->TargetArmLength = math::min(arm-ZoomSpeed, MaxZoomOut);
		}
	}
}
void AGameCharacter::LockOntoTarget(AActor* target) {
	
	if (TargetLockActor != nullptr) { // end previous target lock-on
		GetWorld()->DestroyActor(TargetLockActor);
	}
	if (target == nullptr) { // start new lock-on
		TargetLockActor = nullptr;
	}else{
		FTransform transform = target->GetTransform();
		double3 translate = transform.GetTranslation();
		translate.Z += 20;
		transform.SetTranslation(translate);
		FActorSpawnParameters params;
		
		TargetLockActor = GetWorld()->SpawnActorDeferred<AActor>(WidgetClass, transform,
			(AActor*)nullptr,(APawn*)nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			ESpawnActorScaleMethod::OverrideRootScale
		);
		TargetLockActor->FinishSpawning(transform);
		FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		TargetLockActor->AttachToActor(target, rules);
	}
	ToggleDirectionalMovement(target != nullptr);
}
void AGameCharacter::ToggleDirectionalMovement(bool trueDirectionalMovement) {
	UCharacterMovementComponent * mov = this->GetCharacterMovement();
	mov->bOrientRotationToMovement = trueDirectionalMovement;
	mov->bUseControllerDesiredRotation = !trueDirectionalMovement;
}
UCameraComponent* AGameCharacter::GetCurrentCamera() {
	return FollowCamera->IsActive() ? FollowCamera : FirstPersonCamera;
}
void AGameCharacter::LockOntoEnemy(const FInputActionValue& Value) {
	const float TargetLockDistance = 1000;
	
	if (TargetLockActor==nullptr) {

		FTransform transform = GetCurrentCamera()->GetComponentTransform();
		double3 start = transform.GetLocation();
		start.Z += 20.;
		double3 forward = transform.GetRotation().GetForwardVector();
		double3 end = start + forward * TargetLockDistance;
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
		objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(this);
		FHitResult OutHit; 
		if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), start, end, 150., objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true)) {
			LockOntoTarget(OutHit.GetActor());
		}

	}
	else {
		LockOntoTarget((AActor*)nullptr);
	}
}
void AGameCharacter::OnComboPartEnd(bool isLast) {
	if (isLast) {
		IsAttacking = false;
	}else if (DoNextCombo) {
		DoNextCombo = false;
	}else {
		UAnimInstance* anim = this->GetMesh()->GetAnimInstance();
		anim->Montage_Stop(0.2, AttackComboAnim);
		IsAttacking = false;
	}
}
void AGameCharacter::Interact(const FInputActionValue& Value) {

}
void AGameCharacter::Attack(const FInputActionValue& Value) {
	if (IsAttacking) {
		DoNextCombo = true;
	}
	else {
		UAnimInstance * anim = this->GetMesh()->GetAnimInstance();
		anim->Montage_Play(AttackComboAnim);
		IsAttacking = true;
		DoNextCombo = false;
	}
}

void AGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		if (this->GetCharacterMovement()->IsInWater()) {
			// get forward vector
			const FVector ForwardDirection = Rotation.Quaternion().GetForwardVector();

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Length());
		}
		else {
			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
			
		}
	}
}
void AGameCharacter::SetSwimming(bool isSwimming) {
	
}

void AGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGameCharacter::Tick(float DeltaTime) {
	if (TargetLockActor!=nullptr) {
		double3 playerLocation = GetActorLocation();
		double3 targetLocation = TargetLockActor->GetActorLocation();
		FRotator rot = UKismetMathLibrary::FindLookAtRotation(playerLocation, targetLocation);
		rot.Pitch -= 20.;
		GetController()->SetControlRotation(rot);
	}
	
}