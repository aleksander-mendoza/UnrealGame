// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "blender/utildefines.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"
#include "proc_assets/WorldGen.h"
#include "Kismet/GameplayStatics.h"
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

	USkeletalMeshComponent* playerMesh = GetMesh();

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 0.; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeTransform(FTransform(FVector(0., 0., 40.)));
	FollowCamera->SetActive(false);

	// Create a follow camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(playerMesh, "head"); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FirstPersonCamera->SetRelativeTransform(FTransform(FRotator(0, 90, -90), FVector(0, 10, 0)));
	FirstPersonCamera->bUsePawnControlRotation = true; // Camera rotates relative to head
	FirstPersonCamera->SetActive(true);

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	
	LeftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHandMesh"));
	RightHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHandMesh"));
	LeftHandMesh->SetSimulatePhysics(false);
	RightHandMesh->SetSimulatePhysics(false);
	LeftHandMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RightHandMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	Health = CreateDefaultSubobject<UHealth>(TEXT("Health"));

	Inventory = CreateDefaultSubobject<UActorInventory>(TEXT("PlayerInventory"));
	Inventory->containerEvents = this;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	SetGender(IsFemale);


	

}


void preprocessComboAnim(AGameCharacter *c, UAnimMontage * a) {
	int n = a->Notifies.Num();
	for (int i = 0; i < n; i++) {
		if (UNotifyCombo* notify = Cast<UNotifyCombo>(a->Notifies[i].Notify)) {
			notify->IsLast = i + 1 == n;
		}
	}
}
void preprocessComboAnims(AGameCharacter* c, TArray<TObjectPtr<UAnimMontage>>& a) {
	for (int i = 0; i < a.Num(); i++) {
		preprocessComboAnim(c, a[i]);
	}
}
void AGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	preprocessComboAnims(this, RightHandedStealthAttackComboAnims);
	preprocessComboAnims(this, LeftHandedStealthAttackComboAnims);
	preprocessComboAnims(this, RightHandedAttackComboAnims);
	preprocessComboAnims(this, LeftHandedAttackComboAnims);
	preprocessComboAnims(this, DoubleHandedAttackComboAnims);
	preprocessComboAnims(this, BareLeftHandAttackComboAnims);
	preprocessComboAnims(this, BareRightHandAttackComboAnims);
	USkeletalMeshComponent* playerMesh = GetMesh();
	const FAttachmentTransformRules atr(EAttachmentRule::KeepRelative, false);
	RightHandMesh->RegisterComponent();
	LeftHandMesh->RegisterComponent();
	LeftHandMesh->AttachToComponent(playerMesh, atr, HandSocketL);
	RightHandMesh->AttachToComponent(playerMesh, atr, HandSocketR);
	LeftBareHandSocket = playerMesh->GetSocketByName(HandSocketL);
	RightBareHandSocket = playerMesh->GetSocketByName(HandSocketR);
	
	ToggleDirectionalMovement(true);
	Inventory->ResetToDefault(GetWorld());
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
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
	getAnimInstance()->IsStrafe = trueDirectionalMovement;
}
void AGameCharacter::LockOntoEnemy(const FInputActionValue& Value) {
	
	
	if (TargetLockActor==nullptr) {

		Ray ray;
		getRay(TargetLockDistance, ray);
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
		objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(this);
		FHitResult OutHit; 
		if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), ray.start, ray.end, 150., objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true)) {
			LockOntoTarget(OutHit.GetActor());
		}

	}
	else {
		LockOntoTarget((AActor*)nullptr);
	}
}
void AGameCharacter::InteractStart(const FInputActionValue& Value) {
	Ray ray;
	getRay(InteractDistance, ray);
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
	objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	FHitResult OutHit;
	if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), ray.start, ray.end, objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true)) {
		interactedActor = OutHit.GetActor();
		auto h = OutHit.GetHitObjectHandle();
		auto c = OutHit.GetComponent();
		physicshandleDistance = OutHit.Distance;
		PhysicsHandle->GrabComponentAtLocationWithRotation(c,"",h.GetLocation(),h.GetRotation());
		
	}
}
void AGameCharacter::InteractEnd(const FInputActionValue& Value) {
	interactedActor = nullptr;
	PhysicsHandle->ReleaseComponent();
	physicshandleDistance = -1;
}

void AGameCharacter::Move(const FInputActionValue& Value)
{
	if (!isPlayingAttackAnim()) {
		// input is a Vector2D
		FVector MovementVector = Value.Get<FVector>();

		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			if (this->GetCharacterMovement()->IsInWater()) {
				if (!bIsSwimming) {
					SetSwimming(true);
				}
				// add movement 
				const FVector UpDirection(0, 0, 1);
				AddMovementInput(UpDirection, MovementVector.Z);
			}
			else {
				if (bIsSwimming) {
					SetSwimming(false);
				}



			}
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void AGameCharacter::getRay(double length, Ray& ray) {
	FTransform transform = GetCurrentCamera()->GetComponentTransform();
	ray.start = transform.GetLocation();
	//ray.start.Z += 20.;
	double3 forward = transform.GetRotation().GetForwardVector();
	ray.start += forward * CameraBoom->TargetArmLength;
	ray.end = ray.start + forward * length;
}


double3 AGameCharacter::getRayEnd(double length) {
	Ray ray;
	getRay(length, ray);
	return ray.end;
}

float AGameCharacter::HitDetect(UItemObject* item, const USkeletalMeshSocket* bareHand, const UStaticMeshSocket* start, const UStaticMeshSocket* end, UStaticMeshComponent* mesh, TArray<FHitResult> & OutHit)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
	objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	double3 s, e;
	if (item == nullptr || start == nullptr) {
		check(bareHand != nullptr);
		e = s = bareHand->GetSocketLocation(GetMesh());
		
	}else{
		check(end != nullptr);
		check(start != nullptr);
		FTransform startTrans;
		start->GetSocketTransform(startTrans, mesh);
		FTransform endTrans;
		end->GetSocketTransform(endTrans, mesh);
		s = startTrans.GetLocation();
		e = endTrans.GetLocation();
		
	}
	if (UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), s, e, 50., objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true)) {
		return Health->getDamage(item);
	}
	return 0;
}

void AGameCharacter::OnEquipClothes(TObjectPtr<UItemObject> item)
{
	USkeletalMesh* clothingMesh = item->getSkeletalMesh();
	check(clothingMesh != nullptr);
	check(item->isWearable());
	USkeletalMeshComponent* clothingComp = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass());
	USkeletalMeshComponent* playerMesh = GetMesh();
	clothingComp->RegisterComponent();
	clothingComp->AttachToComponent(playerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
	clothingComp->SetSkeletalMesh(clothingMesh);
	clothingComp->SetAnimInstanceClass(playerMesh->AnimClass);
	clothingComp->SetLeaderPoseComponent(playerMesh, true);
	const int idx = Clothes.Add(clothingComp);
	Health->Defence += item->getItemArmor();
	checkf(item->equippedAt == idx, TEXT("%d != %d"), item->equippedAt, idx);
	check(Inventory->Clothes[item->equippedAt] == item);
}

void AGameCharacter::OnDropItem(TObjectPtr<UItemObject> item) {
	worldGenRef->spawnItem(item, GetActorLocation(), FRotator());
	Health->CarriedWeight -= item->getItemWeight();
}


void AGameCharacter::Tick(float DeltaTime) {
	if (TargetLockActor!=nullptr) {
		double3 playerLocation = GetActorLocation();
		double3 targetLocation = TargetLockActor->GetActorLocation();
		FRotator rot = UKismetMathLibrary::FindLookAtRotation(playerLocation, targetLocation);
		rot.Pitch -= 20.;
		GetController()->SetControlRotation(rot);
	}
	if (physicshandleDistance>0) {
		double3 pos = getRayEnd(physicshandleDistance);
		PhysicsHandle->SetTargetLocation(pos);
	}
	if (bIsRunning) {
		float s = Health->Stamina - 10*DeltaTime;
		Health->PreventStaminaRegen = 1;
		if (s < 0) {
			s = 0;
			endRun();
		}
		Health->setStamina(s);
	}
	if (isPlayingAttackAnim()) {
		if (hitDetectionTimer > 0) {
			hitDetectionTimer -= DeltaTime;
		}
		else {
			HitDetectAll();
			hitDetectionTimer = HIT_DETECTION_PERIOD;
		}
	}else{
		if (AttackCooldown > 0) {
			AttackCooldown -= DeltaTime;
			if (AttackCooldown <= 0) {
				if (wantsToAttackLeftHanded) {
					leftHandedAttackStart();
				}
				else if (wantsToAttackRightHanded) {
					rightHandedAttackStart();
				}
			}
		}
	}
	
	if (invincibilityDuration > 0) {
		invincibilityDuration -= DeltaTime;
	}
	
}


